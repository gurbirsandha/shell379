#include "helpers.hpp"

using namespace std;

void restoreStdout(int stdout) {
    dup2(stdout,1); // redirect stdout back to stdout
    close(stdout);
}

void restoreStdin(int stdin) {
    dup2(stdin,0); // redirect stdin back to stdin
    close(stdin);
}


int redirectStdoutToFile(string filename){
    int stdout = dup(1);
    int f = open(&filename[0], O_WRONLY | O_CREAT | O_CLOEXEC, S_IWUSR | S_IRUSR );
    if (f < 0) {
        perror("error opening file");
    }
    dup2(f, 1); // redirect stdout to file
    close(f);
    return stdout;
}

int redirectStdinToFile(string filename){
    int stdin = dup(0);
    int f = open(&filename[0], O_RDONLY | O_CLOEXEC , S_IRUSR );
    if (f < 0) {
        perror("error opening file");
    }
    dup2(f, 0);// redirect stdin to file
    close(f);
    return stdin;
    
}
string charArrayToString(char** args) {
    string commandString;
    for (int i = 0; i < sizeof(args)/sizeof(char*);++i) {
        string temp(args[i]);
        commandString += temp;
    }
    return commandString;
}

int getSysTime() {
    // resource usage of all completed and waited for children of current process
    struct rusage resource;
    getrusage(RUSAGE_CHILDREN, &resource);
    long sec = resource.ru_stime.tv_sec;
    int total = (int)sec;
    
    return total;
}

int getUserTime() {
    // resource usage of all completed and waited for children of current process
    struct rusage resource;
    getrusage(RUSAGE_CHILDREN, &resource);
    long sec = resource.ru_utime.tv_sec;
    int total = (int)sec;
    
    return total;
}