/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "logger.h"
#include <ctime>
#include <iomanip>
#include <iostream> 

namespace sandbox {
namespace shared {

LogLevel Logger::reporting_level_ = INFO;

const char * const LOG_LEVEL_STRINGS[] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "DEBUG1",
    "DEBUG2",
    "DEBUG3",
    "DEBUG4"
};

Logger::Logger()
{}

Logger::~Logger()
{
    if (instance_level_ >= Logger::getReportingLevel())
    {
        std::cout << output_stream_.str() << std::endl;
    }
}

std::ostringstream& Logger::log(LogLevel level)
{
    output_stream_ << timestamp() << " [" << getLevelString(level) << "] ";
    
    instance_level_ = level;

    return output_stream_;
}

std::string Logger::getLevelString(LogLevel level)
{
    std::string level_string = LOG_LEVEL_STRINGS[level];
    return level_string;
}

std::string Logger::timestamp()
{
    time_t raw_time;  
    struct tm * time_info;  
  
    time(&raw_time);  
    time_info = localtime(&raw_time);
    
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << time_info->tm_hour << ":" << std::setw(2) << std::setfill('0') << time_info->tm_min << ":" << std::setw(2) << std::setfill('0') << time_info->tm_sec;
    
    return os.str();
}

void Logger::setReportingLevel(LogLevel level)
{
    Logger::reporting_level_ = level;
}

LogLevel Logger::getReportingLevel()
{
    return Logger::reporting_level_;
}

}  // namespace sandbox
}  // namespace shared
