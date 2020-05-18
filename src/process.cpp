#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ = pid;
}

int Process::Pid() { return pid_; }


float Process::CpuUtilization() const { return cpu_; }


string Process::Command() { return LinuxParser::Command(Pid()); }


string Process::Ram() { return LinuxParser::Ram(Pid()); }


string Process::User() { return LinuxParser::User(Pid()); }

long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }


void Process::CpuUtilization(long process_ticks,long system_ticks){
    long duration_process = process_ticks -cached_process_ticks_;
    long duration = system_ticks - cached_system_ticks_;
    cpu_ = static_cast<float>(duration_process)/duration;

    cached_process_ticks_ = process_ticks;
    cached_system_ticks_ = system_ticks;
}


bool Process::operator<(Process const& a) const { 
    return (CpuUtilization()<a.CpuUtilization()) ? true : false; }

bool Process::operator>(Process const& a) const { 
    return (CpuUtilization()>a.CpuUtilization()) ?true : false; }