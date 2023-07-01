#pragma once
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	class Texture2D
	{
	protected:
		int _width = 0;
		int _height = 0;
		int _texChannels = 0;
		string _filepath;
	public:
		Texture2D(const std::string& path);

		inline const int& getWidth() const& { return _width; }
		inline const int& getHeight() const& { return _height; }
		inline const int& textureChannelsCount() const& { return _texChannels; }
		virtual void release() = 0;
	};
}