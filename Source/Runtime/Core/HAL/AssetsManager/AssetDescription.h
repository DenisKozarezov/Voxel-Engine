#pragma once
#include <Core/CoreDefines.h>
#include <Core/CoreTypes.h>

namespace assets
{
    enum class AssetType
    {
        Disk = 0,
        Network = 1,
    };
    
    class AssetDescription
    {
    private:
        const string m_path;
        const uint64 m_hash;
        const AssetType m_type;
    public:
        AssetDescription(const std::string& resourcePath, AssetType type = AssetType::Disk);
        AssetDescription(std::string&& resourcePath, AssetType type = AssetType::Disk);

        FORCE_INLINE const string& path() const { return m_path; }
        FORCE_INLINE const uint64& hash() const { return m_hash; }
        FORCE_INLINE const AssetType& type() const { return m_type; }
        
        ~AssetDescription() = default;
    };
}