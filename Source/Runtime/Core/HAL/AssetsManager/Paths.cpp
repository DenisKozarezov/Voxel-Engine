#include "Paths.h"
#include <filesystem>
#include "FileManager.h"

#define RESOURCES_FOLDER_NAME "Resources"

string Paths::launchDir()
{
    return std::filesystem::current_path().string() + PATH_SEPARATOR;
}

string Paths::assetsDir()
{
    return launchDir() + RESOURCES_FOLDER_NAME + PATH_SEPARATOR;
}

string Paths::shaderWorkingDir()
{
    return assetsDir() + "shaders" + PATH_SEPARATOR;
}

string Paths::cacheDir()
{
    return assetsDir() + "cache" + PATH_SEPARATOR;
}

string Paths::getExtension(const string& inPath, bool includeDot)
{
    const string filename = getCleanFilename(inPath);
    const size_t dotPos = filename.find_last_of('.');
    if (dotPos != INDEX_NONE)
    {
        return filename.substr(dotPos + (includeDot ? 0 : 1));
    }
    return "";
}

string Paths::getCleanFilename(const string& inPath)
{
    static_assert(INDEX_NONE == -1, "INDEX_NONE assumed to be -1");

    const size_t endPos = inPath.find_last_not_of(PATH_SEPARATOR) + 1;
    const size_t startPos = inPath.find_last_of(PATH_SEPARATOR) + 1;
    
    string result = startPos <= endPos ? inPath.substr(startPos, endPos - startPos) : "";
    return result;
}

string Paths::getCleanFilename(string&& inPath)
{
    static_assert(INDEX_NONE == -1, "INDEX_NONE assumed to be -1");

    const size_t endPos = inPath.find_last_not_of(PATH_SEPARATOR) + 1;
    const size_t startPos = inPath.find_last_of(PATH_SEPARATOR) + 1;
    
    if (startPos <= endPos)
    {
        inPath.erase(inPath.begin() + startPos, inPath.end() - endPos);
        inPath.erase(inPath.begin(), inPath.begin() + startPos);
    }
    else
    {
        inPath.clear();
    }
    return std::move(inPath);
}

template<typename T>
string getBaseFilenameImpl(T&& inPath, bool removePath)
{
    string cleanPath = removePath ? Paths::getCleanFilename(std::forward<T>(inPath)) : std::forward<T>(inPath);
    const size_t extPos = inPath.find_last_of('.') + 1;

    if (extPos != INDEX_NONE)
    {
        // determine the position of the path/leaf separator
        int32_t leafPos = INDEX_NONE;
        if (!removePath)
        {
            leafPos = inPath.find_last_of(PATH_SEPARATOR) + 1;
        }
    }
    return cleanPath;
}

string Paths::getBaseFilename(const string& inPath, bool removePath)
{
    return getBaseFilenameImpl(inPath, removePath);
}

string Paths::getBaseFilename(string&& inPath, bool removePath)
{
    return getBaseFilenameImpl(std::move(inPath), removePath);
}

string Paths::getPath(const string& inPath)
{
    const size_t pos = inPath.find_last_of(PATH_SEPARATOR) + 1;

    string result;
    if (pos != INDEX_NONE)
    {
        result = inPath.substr(0, pos);
    }
    return result;
}

string Paths::getPath(string&& inPath)
{
    const size_t pos = inPath.find_last_of(PATH_SEPARATOR) + 1;

    string result;
    if (pos != INDEX_NONE)
    {
        inPath.erase(inPath.begin() + pos, inPath.end() - pos);
        result = std::move(inPath);
    }
    return result;
}

bool Paths::fileExists(const string& inPath)
{
    return FileManager::fileExists(inPath);
}

bool Paths::directoryExists(const string& inPath)
{
    return FileManager::directoryExists(inPath);
}
