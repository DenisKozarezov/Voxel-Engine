#include "Asset.h"
#include <Core/Logging/Log.h>

namespace assets
{
    Asset::Asset(const AssetDescription& assetDesc)
        : m_assetDesc(assetDesc)
    {
        RUNTIME_WARN("Registering the asset '{0}' (hash: {1})...", m_assetDesc.path(), m_assetDesc.hash());
    }

    bool Asset::loadFromFile()
    {
        if (!m_isLoaded)
        {
            m_isLoaded = loadImpl();
        }
        return m_isLoaded;
    }

    void Asset::unload()
    {
        if (m_isLoaded)
        {
            RUNTIME_WARN("Unregistering the asset '{0}' (hash: {1})...", m_assetDesc.path(), m_assetDesc.hash());
            unloadImpl();
        }
        
        m_isLoaded = false;
    }
}
