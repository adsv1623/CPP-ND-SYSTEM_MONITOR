#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() { 
    float active = float(LinuxParser::ActiveJiffies());
    float total =  float(LinuxParser::Jiffies());
    return active/total; }