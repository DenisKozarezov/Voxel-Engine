#pragma once
#include <Core/CoreTypes.h>
#include "Core/Base.h"

class FileManager
{
protected:
    FileManager() = default;
    virtual ~FileManager() = default;
public:
    static bool deleteFile(const string& filepath);
    static bool moveFile(const string& destination, const string& source);
    FORCE_INLINE static bool fileExists(const string& filepath);
    FORCE_INLINE static bool directoryExists(const string& directory);
    static bool makeDirectory(const string& path);
    static bool deleteDirectory(const string& directory);
    [[nodiscard]] FORCE_INLINE virtual uint64 fileSize(const string& filepath);

    static SharedRef<std::ifstream> createFileReader(const string& filepath);
    static SharedRef<std::ofstream> createFileWriter(const string& filepath);
};
