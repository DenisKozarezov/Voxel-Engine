#pragma once
#include <version.h>
#include "Application.h"

extern UniqueRef<VoxelEngine::Application> CreateApplication(ApplicationCommandLineArgs args);

#ifdef VOXEL_PLATFORM_WINDOWS
int main(int argc, char** argv)
{
    VoxelEngine::Log::init(PROJECT_NAME, "Editor Application");

    VOXEL_CORE_TRACE("============ Application Entry Point ============");
    VOXEL_CORE_WARN("Initializing Logging System...");

    auto app = CreateApplication({ argc, argv });
    app->run();
    app->shutdown();

    VOXEL_CORE_TRACE("================ Application QUIT ===============");
    return 0;
}
#endif