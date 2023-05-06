#pragma once
#include "core/Application.h"
#include "core/Log.h"

#ifdef VOXEL_PLATFORM_WINDOWS

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