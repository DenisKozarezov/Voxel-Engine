#pragma once
#include "Shader.h"

namespace VoxelEngine::renderer
{
    class IMaterial
    {
    private:
        TSharedPtr<Shader> m_shader;
        bool m_instanced;
    public:
        IMaterial(const bool& instanced = false) : m_instanced(instanced) { }

        void setShader(const TSharedPtr<Shader>& shader);
        FORCE_INLINE const TSharedPtr<Shader>& getShader() const { return m_shader; }
        
        FORCE_INLINE const bool& instanced() const { return m_instanced; }
        virtual void bind() const = 0;

        virtual ~IMaterial() = default;
    };
}