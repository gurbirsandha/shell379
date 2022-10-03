#include "shell379PCB.hpp"

class Shell379 {
    private: 
        static void removeFromPCB(int why);
        static map<int, Shell379PCB> PCtable;
        int getProcessTime(int pid);
        void jobsCommand(string filename);
        void sleepCommand(int sleepTime);
        void killCommand(int pid);
        void resumeCommand(int pid);
        void suspendCommand(int pid);
        void waitCommand(int pid);
        void exitCommand(string filename);
        void executeGenericCommand(char * args[], bool inBackground, string inFile, string outFile);
    public:
        void runShell();

};