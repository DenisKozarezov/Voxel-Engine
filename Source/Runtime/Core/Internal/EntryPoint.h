#pragma once
#include "Application.h"
#include <version.h>

extern TUniquePtr<VoxelEngine::Application> CreateApplication(ApplicationCommandLineArgs args);

#ifdef VOXEL_PLATFORM_WINDOWS
int main(int argc, char** argv)
{
    VoxelEngine::Log::init(PROJECT_NAME, "Editor Application");

    RUNTIME_TRACE("============ Application Entry Point ============");
    RUNTIME_WARN("Initializing Logging System...");

    auto app = CreateApplication({ argc, argv });
    app->run();
    app->shutdown();

    RUNTIME_TRACE("================ Application QUIT ===============");
    return 0;
}
#endif