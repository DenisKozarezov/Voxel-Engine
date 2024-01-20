#include "IMaterial.h"

namespace VoxelEngine::renderer
{
    void IMaterial::setShader(const TSharedPtr<Shader>& shader)
    {
        m_shader = shader;
    }
}
