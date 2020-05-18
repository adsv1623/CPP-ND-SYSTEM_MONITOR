#include <string>
#include "format.h"
#include <iomanip>
using std::string;

std::string Format::ElapsedTime(long seconds) { 

    const int SECPERMIN =60;
    const int SECPERHOUR =3600;
     
    long lseconds = seconds % SECPERMIN;
    long lminutes = (seconds / SECPERMIN) %SECPERMIN;
    long lhours   = seconds /SECPERHOUR; 

    std::stringstream ssresult;

    ssresult << std::setfill('0') << std::setw(2) << lhours << ":";
    ssresult << std::setfill('0') << std::setw(2) << lminutes << ":";  
    ssresult << std::setfill('0') << std::setw(2) << lseconds; 

    return ssresult.str();
}
