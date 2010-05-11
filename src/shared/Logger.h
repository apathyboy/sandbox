/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <sstream> 
#include <string>

namespace sandbox {
namespace shared {

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

}  // namespace sandbox
}  // namespace shared

#endif // LOGGER_H_