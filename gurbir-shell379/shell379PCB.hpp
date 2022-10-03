#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <sys/wait.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>

using namespace std;

#define LINE_LENGTH 100 // Max # of characters in an input line
#define MAX_ARGS 7 // Max number of arguments to a command
#define MAX_LENGTH 20 // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

class Shell379PCB {
    // stores status and command entered by user for each process
    private:
        char status;
        string command;
    public:
        Shell379PCB() = default;
        void setStatus(char status);
        void setCommand(string arg);
        string getCommand();
        char getStatus();
};