#pragma once
#include "Asset.h"
#include <vector>

namespace assets
{
    class ShaderAsset : public Asset
    {
    private:
        TSharedPtr<std::ifstream> m_handle;
        std::streamsize m_size;
        
        bool loadImpl() override;
        void unloadImpl() override;
    public:
        ShaderAsset(const AssetDescription& assetDesc);
        ~ShaderAsset() override = default;

        string readProgram() const;
        std::vector<uint32> readBinaries() const;
    };
}