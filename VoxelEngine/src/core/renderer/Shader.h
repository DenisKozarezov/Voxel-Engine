#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>

enum ShaderStage : byte { Vertex, Fragment, Geometry, Compute };

namespace VoxelEngine::renderer
{
	class Shader
	{
	private:
		string m_name;
		string m_filepath;
		ShaderStage m_shaderStage;
	public:
		Shader() noexcept = delete;
		Shader(const string& filepath, const ShaderStage& shaderStage);
		Shader(const string& vertexPath, const string& fragmentPath, const string& geomertryPath = nullptr);
		Shader(Shader const&) noexcept = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(Shader const& rhs) noexcept = delete;
		Shader& operator=(Shader&& rhs) noexcept = delete;

		inline constexpr string getName() noexcept { return m_name; }
		virtual void unbind() const = 0;

		virtual ~Shader() = default;
	protected:
		string readFile(const string& filename);
	};
}