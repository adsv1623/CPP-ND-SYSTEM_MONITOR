#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include <stdexcept>
#include <iostream>
#include <bits/stdc++.h>
#include <vector>

using std::vector;
using std::stof;
using std::string;
using std::to_string;
using std::vector;
// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel,version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  string line,key,val;
  vector<float> V;
  if(filestream.is_open()){
    while (std::getline(filestream,line))
    {
      std::istringstream stream(line);
      stream>>key>>val;
      if(key=="MemTotal:"||key=="MemFree:"||key=="Buffers:"){
        V.push_back(std::stof(val));}
      if(V.size()==2){break;}
    }
    
  }
	return 1.0-(V[1]/(V[0]-V[2]));
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory+kUptimeFilename);
  string line,val{""};
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream>>val; 
  }
  return std::stol(val);
}


// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies()+IdleJiffies();
}

// Done
long LinuxParser::ActiveJiffies(int pid) {
  string line,val;
  int i;
  long active_j{0};
  std::ifstream file(kProcDirectory+to_string(pid)+kStatFilename);
  if(file.is_open()){
    std::getline(file,line);
    std::istringstream stream(line);
    for(i=1;i<18;i++){
      stream >> val;
      if(14<=i&&i<=17)
        active_j+=std::stoi(val);
    }
  }
  return active_j; }

long LinuxParser::ActiveJiffies() { 
  long active_J{0};
  vector<CPUStates> active_states= {
  kUser_,
  kNice_,
  kSystem_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  };
  vector <string> utilization =CpuUtilization();
  for(auto state:active_states){
    active_J += std::stol(utilization[state]); 
  }
  return active_J; }

long LinuxParser::IdleJiffies() { 
  long idle_j{0};
  vector<CPUStates> idle_states={
    kIdle_,
    kIOwait_,
  };
  vector<string> utilization = CpuUtilization();
  for(auto state:idle_states){
    idle_j += std::stol(utilization[state]);
  }
  return idle_j; }

vector<string> LinuxParser::CpuUtilization() { 
  string line,val;
  vector <string> cpuVector;
  std::ifstream file(kProcDirectory+kStatFilename);
  if(file.is_open()){
    std::getline(file,line);
    std::istringstream stream(line);
    while(stream >>val){
      if(val!="cpu"){
        cpuVector.emplace_back(val);
      }
    };
  }
  return cpuVector;
}


int LinuxParser::TotalProcesses() { 
  std::string line,key,val;
  std::ifstream file(kProcDirectory+kStatFilename);
  if(file.is_open()){
    while(std::getline(file,line)){
      std::istringstream stream(line);
      stream>>key>>val;
      if(key=="processes"){ return std::stoi(val);}
    }
  }
  return 0; }


int LinuxParser::RunningProcesses() { 
  std::string line,key,val;
  std::ifstream file(kProcDirectory+kStatFilename);
  if(file.is_open()){
    while(std::getline(file,line)){
      std::istringstream stream(line);
      stream>>key>>val;
      if(key=="procs_running"){ return std::stoi(val);}
    }
  }
  return 0; }


string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream file(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if(file.is_open()){
    std::getline(file,line);
    return line;
  }
  return ""; }

string LinuxParser::Ram(int pid) { 
  string line,key,value;
  std::ifstream file(kProcDirectory+to_string(pid)+kStatusFilename);
  if(file.is_open()){
    while(std::getline(file,line)){
      std::istringstream stream(line);
      stream >>key >> value;
      if(key=="VmSize:"){
        int temp = (100*std::stof(value)/1024+0.5);
        return to_string(temp/100)+"."+((temp%100<10) ? "0"+to_string(temp%100):to_string(temp%100));
      }; }}
  return "0"; }


string LinuxParser::Uid(int pid) { 
  string line,key,value;
  std::ifstream file(kProcDirectory+to_string(pid)+kStatusFilename);
  if(file.is_open()){
    while(std::getline(file,line)){
      std::istringstream stream(line);
      stream >>key >> value;
      if(key=="Uid:"){
        break;
      }
    }
  }
  return value; }


string LinuxParser::User(int pid) {
  string line;
  string search_token = "x:"+to_string(pid);
  std::ifstream file(kPasswordPath);
  if(file.is_open()){
    while(std::getline(file,line)){
      auto pos = line.find(search_token);
      if(pos!=string::npos)
        return line.substr(0,pos-1);
    }
  }
  return "root"; }


long LinuxParser::UpTime(int pid) { 
  string line,hlp;
  std::ifstream file(kProcDirectory+to_string(pid)+kStatFilename);
  if(file.is_open()){
    std::getline(file,line);
    std::istringstream stream(line);
    std::istream_iterator<string> begin(stream),end;
    vector<string> line_content(begin,end);
    return (LinuxParser::UpTime()-float(stof(line_content[21])/sysconf(_SC_CLK_TCK)));
  }
  return 0; }