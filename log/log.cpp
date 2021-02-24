//
// Created by 19047 on 2021/2/17.
//

#include "log.h"
#include <iostream>
#include <ctime>
#include <thread>

namespace Logger{
    std::string LogLevel::toString(LogLevel::Level level) {
        switch (level) {
#define XX(name) \
            case (LogLevel::name): \
            return #name; \
            break;

        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
        XX(UNKNOW);
#undef XX
            default:
                return "UNKOWN";
        }
        return "UNKOWN";
    }

    Log::Log(LogLevel::Level level, std::thread::id threadId, std::string content) {
        LogLevel logLevel;
        std::string str1 = logLevel.toString(level);

        char dateTime[50];
        time_t now = time(NULL);
        strftime(dateTime, 50, "%x %X", localtime(&now));

        std::cout << dateTime << " " << threadId << " [" << str1 << "]:" << content << std::endl;
    }
}
