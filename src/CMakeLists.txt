target_sources(${PROJECT_NAME} PRIVATE
        Engine.cpp
        Commands.cpp
        Plugins.cpp
        SystemLogger.cpp
        SystemUserInput.cpp
        SystemDearImGui.cpp
        SystemWindowGLFW.cpp
        SystemTimer.cpp
        SystemParallelProcessing.cpp
        SystemVulkan.cpp
        SystemPlugins.cpp
        SystemCommandBuffers.cpp
        SystemPlatform.cpp
        main.cpp)