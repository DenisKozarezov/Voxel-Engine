#include "Asset.h"
#include <Core/Logging/Log.h>

namespace assets
{
    Asset::Asset(const AssetDescription& assetDesc)
        : m_assetDesc(assetDesc)
    {
        RUNTIME_WARN("Registering the asset '{0}' (hash: {1})...", m_assetDesc.path(), m_assetDesc.hash());
    }

    Asset::~Asset()
    {
        RUNTIME_WARN("Unregistering the asset '{0}' (hash: {1})...", m_assetDesc.path(), m_assetDesc.hash());
    }
}
