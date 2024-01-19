#pragma once
#include <filesystem>
#include <fstream>
#include <Core/CoreTypes.h>
#include <Core/CoreDefines.h>

class FileManager
{
protected:
    FileManager() = default;
    virtual ~FileManager() = default;
public:
    static bool deleteFile(const string& filepath);
    static bool moveFile(const string& destination, const string& source);
    static bool fileExists(const string& filepath);
    static bool directoryExists(const string& directory);
    static bool makeDirectory(const string& directory);
    static bool deleteDirectory(const string& directory);
    NODISCARD virtual uint64 fileSize(const string& filepath);

    static TSharedPtr<std::ifstream> createFileReader(const string& filepath);
    static TSharedPtr<std::ofstream> createFileWriter(const string& filepath);
};
