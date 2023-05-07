#pragma once
#include "Application.h"
#include "Log.h"

#ifdef VOXEL_PLATFORM_WINDOWS

// Defined in CLIENT!
extern std::unique_ptr<VoxelEngine::Application> CreateApplication(int argc, char** argv);

int main(int argc, char** argv)
{
    VoxelEngine::Log::init();

    VOXEL_CORE_WARN("Initializing Logging System...")

    auto app = CreateApplication(argc, argv);
    app->init();
    app->run();
    return 0;
}
#endif