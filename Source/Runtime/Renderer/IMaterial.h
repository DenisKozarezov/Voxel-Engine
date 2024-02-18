#pragma once
#include <Core/CoreDefines.h>

namespace VoxelEngine::renderer
{
    class IMaterial
    {
    private:
        bool m_instanced;
    public:
        IMaterial(const bool& instanced = false) : m_instanced(instanced) { }
        
        FORCE_INLINE const bool& instanced() const { return m_instanced; }
        virtual void bind() const = 0;

        virtual ~IMaterial() = default;
    };
}