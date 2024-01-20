#pragma once
#include "AssetDescription.h"

namespace assets
{
    class Asset
    {
    private:
        AssetDescription m_assetDesc;
    public:
        Asset(const AssetDescription& assetDesc);
        Asset(const Asset&) = delete;
        Asset(Asset&&) = delete;
        Asset& operator=(const Asset& rhs) = delete;
        Asset& operator=(Asset&& rhs) = delete;
        
        virtual ~Asset();

        FORCE_INLINE const AssetDescription& description() const { return m_assetDesc; }
    };
}