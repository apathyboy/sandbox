/*
 * OpenSWG Combat Upgrade Sandbox - Logger.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <sstream> 
#include <string>

enum LogLevel 
{
    ERR = 0,
    WARNING,
    INFO,
    DEBUG,
    DEBUG1,
    DEBUG2,
    DEBUG3,
    DEBUG4
};

class Logger
{
public:
    Logger();
    virtual ~Logger();
    
    std::ostringstream& log(LogLevel level);
    std::string getLevelString(LogLevel level);

public:
    static void setReportingLevel(LogLevel level = INFO);
    static LogLevel getReportingLevel();

private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    std::string timestamp();

    LogLevel instance_level_;
    static LogLevel reporting_level_;

    std::ostringstream output_stream_;
};

#endif // LOGGER_H_