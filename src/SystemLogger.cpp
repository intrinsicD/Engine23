//
// Created by alex on 27.11.23.
//

#include "SystemLogger.h"
#include "Engine.h"
#include "Commands.h"

namespace Bcg::System::Logger{
    void add_system(){
        auto *engine = Engine::Instance();
        if(!engine->state.ctx().find<LogLevel>()){
            engine->state.ctx().emplace<LogLevel>();
            Log::Info("SystemLogger: Added LogLevel::Info to Context").enqueue();

            set_log_level(LogLevel::Info);
        }
    }

    void remove_system(){
        Log::Info("SystemLogger: Removed").enqueue();
        Log::TODO("SystemLogger: figure our enabling and disabling logging").enqueue();
    }

    void set_log_level(LogLevel level){
        auto *engine = Engine::Instance();
        auto &logLevel = engine->state.ctx().get<LogLevel>();
        logLevel = level;
        std::string level_str;
        switch (level) {
            case LogLevel::Info:
                level_str = "Info";
                break;
            case LogLevel::Warn:
                level_str = "Warning";
                break;
            case LogLevel::Error:
                level_str = "Error";
                break;
        }
        Log::Info("SystemLogger: Set LogLevel to " + level_str).enqueue();
    }
}