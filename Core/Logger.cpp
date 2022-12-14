#include "Logger.hpp"

namespace Core
{
    void Logger::Init()
    {
        //Add colorful terminal logging
        el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

        //Configure logger
        el::Configurations defaultConf;
        defaultConf.setToDefault();
        defaultConf.setGlobally(el::ConfigurationType::Format, "[%level] %datetime{%h:%m:%s} %msg");
        el::Loggers::reconfigureLogger("default", defaultConf);
    }
}