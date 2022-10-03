#include "shell379PCB.hpp"

void Shell379PCB::setStatus(char status) {
    this->status = status;
}
void Shell379PCB::setCommand(string arg) {
    this->command = arg;
}

string Shell379PCB::getCommand() {
    return this->command;
}

char Shell379PCB::getStatus() {
    return this->status;
}