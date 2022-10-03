#include "shell379.hpp"
#include "helpers.hpp"
map<int,Shell379PCB> Shell379::PCtable;



int Shell379::getProcessTime(int pid) {
    // use ps and pipe to get up-to-date process time
    string commandString = "ps -o time= " + to_string(pid);
    string timeStamp;
    char * psCommand = &commandString[0];
    FILE * p; int c;
    if((p=popen(psCommand,"r")) == NULL) {
        perror("error opening pipe");
    } else {
        while ((c=fgetc(p)) != EOF) {
            timeStamp.push_back(c);
        }
        pclose(p);
    }
    // convert HH:MM:SS timestamp to seconds
    char * tok = strtok(&timeStamp[0],":");
    int seconds = 3600*stoi(tok);
    tok = strtok(NULL,":");
    seconds += 60*stoi(tok);
    tok = strtok(NULL,":");
    seconds += stoi(tok);
    return seconds;
}

void Shell379::jobsCommand(string filename) {
    int stdout;
    if (filename != "") {
        stdout = redirectStdoutToFile(filename);
    }
    int sec; char status; string command;
    int sysTime = getSysTime();
    int userTime = getUserTime();
    int i = -1;
    printf("Running processes:\n");
    printf("#    PID      S   SEC   COMMAND\n");
    for (auto & process: PCtable) {
        if (process.first > 0) {
            i++;
            sec = getProcessTime(process.first); // runs ps to get current process time

            // retrieves status and command from process control table
            status = (process.second).getStatus();
            command = (process.second).getCommand();
            
            printf("%d:   %d   %c    %d      %s &\n", i, process.first, status, sec, &command[0] );
        }
    }
    printf("Processes =     %d active\n", i+1);
    printf("Completed processes:\n");
    
    printf("User time =     %d seconds\n", userTime);
    printf("Sys time =      %d seconds\n", sysTime);
    
    if (filename != "") { // redirect stdout back to stdout
        restoreStdout(stdout);
    }
    return;
}


void Shell379::removeFromPCB(int what) {
    int status;
    int pid;
    //waiting for all children that have terminated and removing them from the process control table
    while ((pid=waitpid(-1,NULL,WNOHANG)) > 0) {
        PCtable.erase(pid);
    }
    
}

void Shell379::exitCommand(string filename){
    int stdout;
    if (filename != "") {
        stdout = redirectStdoutToFile(filename);
    }
    int status = 0; int waiter;
    for (auto & process: PCtable) {
        if (process.first > 0) {
            if ((process.second).getStatus() == 'S') {
                printf("killing suspended process with process ID %d\n", process.first);
                if(kill(process.first,SIGKILL) < 0) {
                    perror("error in kill");
                }
            }
        }
    }
    while ((waiter = wait(&status)) > 0); // wait for all running processes to finish
    int sysTime = getSysTime();
    int userTime = getUserTime();
    printf("Resources used\n");
    printf("User time =     %d seconds\n", userTime);
    printf("Sys time =      %d seconds\n", sysTime);
    if (filename != "") {
        restoreStdout(stdout); // redirect stdout back to stdout
    }
    _exit(0);
    return;

}

void Shell379::sleepCommand(int sleepTime) {
    if(sleep(sleepTime) < 0) {
        perror("error in sleep");
    }
}

void Shell379::waitCommand(int pid) {
    int status;
    if(waitpid(pid,&status,0) < 0) {
        perror("error in waitpid");
    }
}

void Shell379::suspendCommand(int pid) {
    if(kill(pid,SIGSTOP) < 0){
        perror("error in suspend");
    } else {
        PCtable[pid].setStatus('S');
    }
}

void Shell379::resumeCommand(int pid) {
   if(kill(pid,SIGCONT) < 0) {
        perror("error in resume");
   } else {
        PCtable[pid].setStatus('R');
   }
}

void Shell379::killCommand(int pid) {
    // removing process from process control table is handled by SIGCHLD handler
    if(kill(pid,SIGKILL) < 0) {
        perror("error in kill");
    } 
}

void Shell379::executeGenericCommand(char * args[], bool inBackground, string inFile, string outFile) {
    pid_t pid = fork(); int status;
    
    if (pid < 0) {
        perror("error forking");
    } else if (pid == 0) {

        if (inFile != "") {
            redirectStdinToFile(inFile);
        }
        if (outFile != "") {
            redirectStdoutToFile(outFile);
        }

        if (execvp(args[0],args) < 0) {
            perror("error in execvp");
            _exit(0);
        }
        
    } else {
        if (!inBackground) {
            waitpid(pid, &status, 0); // wait for specific foreground process
        } else {
            // store process info
            Shell379PCB processEntry = Shell379PCB();
            string argString = charArrayToString(args);
            // display command for jobs table
            if (inFile != "") {
                argString += " <" + inFile;
            }
            if (outFile != "") {
                argString += " >" + outFile;
            }
            processEntry.setCommand(argString);
            processEntry.setStatus('R');
            PCtable[pid] = processEntry;
        }
    }
    return;
}

void Shell379::runShell() {
    // remove children processes from the process control block as they finish executing
    signal(SIGCHLD,removeFromPCB);

    // overall shell prompt loop
    while (true) {
        cout << "SHELL379: ";
        char input[LINE_LENGTH]; // entire input line
        cin.getline(input, LINE_LENGTH);
        char * args[MAX_ARGS + 2]; // store space-separated tokens

        if (input == string("")) { //handles user pressing enter key in shell
             continue;}

        char * tok = strtok(input, " ");
        args[0] = tok;
        int i = 1;
        bool inBackground = false;
        string inFile = "";
        string outFile = "";

        while (tok != nullptr) {
            tok = strtok(NULL, " ");
            args[i] = tok;
            if (tok != nullptr) {
                if (*tok == '<') { // input file specified
                    inFile = args[i];
                    inFile = inFile.substr(1);
                    args[i] = NULL; // remove from arguments to be passed to exec call later
                } else if (*tok == '>') { //output file specified
                    outFile = args[i];
                    outFile = outFile.substr(1);
                    args[i] = NULL; // remove from arguments to be passed to exec call later
                } else if (*tok == '&') {
                    inBackground = true;
                    args[i] = NULL; // remove from arguments to be passed to exec call later
                }
            }
            i++;
        }


        if (!strcmp(args[0],(char*)"sleep")) {
            sleepCommand(stoi(args[1]));
        } 
        else if (!strcmp(args[0],(char*)"kill")) {
            killCommand(stoi(args[1]));
        } 
        else if (!strcmp(args[0],(char*)"suspend")) {
            suspendCommand(stoi(args[1]));
        } 
        else if (!strcmp(args[0],(char*)"resume")) {
            resumeCommand(stoi(args[1]));
        } 
        else if (!strcmp(args[0],(char*)"wait")) {
            waitCommand(stoi(args[1]));
        } 
        else if (!strcmp(args[0],(char*)"jobs")) {
            jobsCommand(outFile);
        } 
        else if (!strcmp(args[0],(char*)"exit")) {
            exitCommand(outFile);
        } 
        else {
            executeGenericCommand(args, inBackground, inFile, outFile);
        }
    }
}
