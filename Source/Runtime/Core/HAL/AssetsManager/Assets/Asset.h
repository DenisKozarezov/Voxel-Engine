#pragma once
#include "AssetDescription.h"

namespace assets
{
    class Asset
    {
    private:
        bool m_isLoaded = false;
        AssetDescription m_assetDesc;
    protected:
        virtual bool loadImpl() = 0;
        virtual void unloadImpl() = 0;
    public:
        Asset(const AssetDescription& assetDesc);
        Asset(const Asset&) = delete;
        Asset(Asset&&) = delete;
        Asset& operator=(const Asset& rhs) = delete;
        Asset& operator=(Asset&& rhs) = delete;

        FORCE_INLINE const bool& isLoaded() const { return m_isLoaded; }
        bool loadFromFile();
        void unload();
        
        virtual ~Asset() = default;

        FORCE_INLINE const AssetDescription& description() const { return m_assetDesc; }
    };
}