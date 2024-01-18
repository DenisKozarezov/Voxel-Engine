#include "FileManager.h"
#include <filesystem>
#include <fstream>
#include <Core/Logging/Assert.h>

bool FileManager::deleteFile(const string& filepath)
{
    if (fileExists(filepath))
    {
        return std::filesystem::remove(filepath);
    }
    return false;
}

bool FileManager::moveFile(const string& destination, const string& source)
{
    return true;
}

bool FileManager::fileExists(const string& filepath)
{
    return std::filesystem::is_regular_file(filepath);
}

bool FileManager::directoryExists(const string& directory)
{
    return std::filesystem::is_directory(directory);
}

bool FileManager::makeDirectory(const string& path)
{
    return std::filesystem::create_directory(path);
}

bool FileManager::deleteDirectory(const string& directory)
{
    if (directoryExists(directory))
    {
        return std::filesystem::remove(directory);
    }
    return false;
}

uint64 FileManager::fileSize(const string& filepath)
{
    return std::filesystem::file_size(filepath);
}

SharedRef<std::ifstream> FileManager::createFileReader(const string& filepath)
{
    constexpr auto readMode = std::ios::in | std::ios::binary;
    auto stream = std::make_shared<std::ifstream>(filepath, readMode);
    VOXEL_CORE_ASSERT(stream->is_open(), "Unable to create a file reader!");
    return stream;
}

SharedRef<std::ofstream> FileManager::createFileWriter(const string& filepath)
{
    constexpr auto writeMode = std::ios::out | std::ios::binary;
    auto stream = std::make_shared<std::ofstream>(filepath, writeMode);
    VOXEL_CORE_ASSERT(stream->is_open(), "Unable to create a file writer!");    
    return stream;
}
