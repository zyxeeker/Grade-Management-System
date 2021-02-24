//
// Created by 19047 on 2021/2/17.
//

#ifndef WEBSEVER_LOG_H
#define WEBSEVER_LOG_H

#include <string>
#include <thread>

namespace Logger{
    class LogLevel{
    public:
        enum Level {
            UNKNOW,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };
        std::string toString(LogLevel::Level level);
    };

    class Log{
    public:
        Log(LogLevel::Level level, std::thread::id ThreadId,std::string content);

    private:
        std::string m_content;
        std::string m_level;



    };

}
#endif //WEBSEVER_LOG_H
