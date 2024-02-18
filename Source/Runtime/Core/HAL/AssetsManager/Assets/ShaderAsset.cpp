#include "ShaderAsset.h"
#include <Core/Logging/Assert.h>
#include <Core/HAL/AssetsManager/FileManager.h>

namespace assets
{
    ShaderAsset::ShaderAsset(const AssetDescription& assetDesc) : Asset(assetDesc)
    {
        
    }
    
    string ShaderAsset::readProgram() const
    {
        string shaderSource;
        shaderSource.resize(m_size);
        m_handle->seekg(0, std::ios::beg);
        m_handle->read(shaderSource.data(), m_size);
        return shaderSource;
    }

    std::vector<uint32> ShaderAsset::readBinaries() const
    {
        std::vector<uint32> shaderSource;
        shaderSource.resize(m_size / sizeof(uint32));
        m_handle->seekg(0, std::ios::beg);
        m_handle->read((char*)shaderSource.data(), m_size);
        return shaderSource;
    }

    bool ShaderAsset::loadImpl()
    {
        m_handle = FileManager::createFileReader(description().path());		
        m_handle->seekg(0, std::ios::end);
        m_size = m_handle->tellg();
        
        RUNTIME_ASSERT(m_size >= 0, "failed to read shader file at path: " + description().path());        
        return true;
    }

    void ShaderAsset::unloadImpl()
    {
        m_handle->close();
        m_size = 0;
    }
}
