#include "AssetDescription.h"
#include <filesystem>

namespace assets
{
    uint64 computeHash(const string& filename)
    {
        constexpr std::hash<string> hasher;
        return hasher(filename);
    }
    
    AssetDescription::AssetDescription(const std::string& resourcePath, AssetType type)
        : m_path(resourcePath), m_hash(computeHash(resourcePath)), m_type(type)
    {
        
    }

    AssetDescription::AssetDescription(std::string&& resourcePath, AssetType type)
        : m_path(std::move(resourcePath)), m_hash(computeHash(resourcePath)), m_type(type)
    {
        
    }
}