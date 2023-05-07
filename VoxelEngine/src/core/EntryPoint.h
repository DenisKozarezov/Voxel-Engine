#pragma once
#include "Application.h"

#ifdef VOXEL_PLATFORM_WINDOWS
extern VoxelEngine::UniqueRef<VoxelEngine::Application> CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    VoxelEngine::Log::init();

    VOXEL_CORE_TRACE("============ Application Entry Point ============")
    VOXEL_CORE_WARN("Initializing Logging System...")

    auto app = CreateApplication({ argc, argv });
    app->init();
    app->run();

    VOXEL_CORE_TRACE("================ Application QUIT ===============")
    return 0;
}
#endif