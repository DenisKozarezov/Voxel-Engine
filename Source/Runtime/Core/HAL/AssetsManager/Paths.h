#pragma once
#include <Core/CoreDefines.h>
#include <Core/CoreTypes.h>

#if defined(VOXEL_PLATFORM_WINDOWS)
#define PATH_SEPARATOR "\\"
#else 
#define PATH_SEPARATOR "/"
#endif

/* Path helpers for retrieving engine dir, etc. */
class Paths
{
public:
    // Returns the directory the application was launched from (useful for commandline utilities)
    static string launchDir();

    // Returns the directory where all resources (models, shaders, etc) are stored
    static string assetsDir();

    // Returns the directory where all shaders are stored
    static string shaderWorkingDir();

    // Returns the directory used for storing cached files used by application in runtime
    static string cacheDir();
    
    /**
     * Gets the extension for this filename.
     *
     * @param	includeDot		if true, includes the leading dot in the result
     *
     * @return	the extension of this filename, or an empty string if the filename doesn't have an extension.
     */
    static string getExtension(const string& inPath, bool includeDot = false);

    // Returns the filename (with extension), minus any path information.
    static string getCleanFilename(const string& inPath);

    // Returns the filename (with extension), minus any path information.
    static string getCleanFilename(string&& inPath);

    // Returns the same thing as GetCleanFilename, but without the extension
    static string getBaseFilename(const string& inPath, bool removePath = true);

    // Returns the same thing as GetCleanFilename, but without the extension
    static string getBaseFilename(string&& inPath, bool removePath = true);

    // Returns the path in front of the filename
    static string getPath(const string& inPath);

    // Returns the path in front of the filename
    static string getPath(string&& inPath);

    // Returns true if this file was found, false otherwise
    static bool fileExists(const string& inPath);

    // Returns true if this directory was found, false otherwise
    static bool directoryExists(const string& inPath);
};
