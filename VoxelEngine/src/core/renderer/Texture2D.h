#pragma once
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	class Texture2D
	{
	protected:
		uint32 _width = 0;
		uint32 _height = 0;
		int _texChannels = 0;
		string _filepath;
	public:
		Texture2D(const std::string& path);

		inline const uint32& getWidth() const& { return _width; }
		inline const uint32& getHeight() const& { return _height; }
		inline const int& textureChannelsCount() const& { return _texChannels; }
		virtual void release() = 0;
	};
}