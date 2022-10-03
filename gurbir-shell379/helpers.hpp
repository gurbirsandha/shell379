#ifndef HELPERS_H
#define HELPERS_H
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/resource.h>
#include <fcntl.h>

using namespace std;

void restoreStdout(int stdout);
void restoreStdin(int stdin);
int redirectStdoutToFile(string filename);
int redirectStdinToFile(string filename);
string charArrayToString(char** args);
int getSysTime();
int getUserTime();

#endif