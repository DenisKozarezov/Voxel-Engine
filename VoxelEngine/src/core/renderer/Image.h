#pragma once
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	enum class ImageFormat
	{
		None = 0,
		RGB,
		RGBA,
		RGBA32F
	};

	class Image
	{
	protected:
		uint32 _width = 0, _height = 0;
		ImageFormat _format = ImageFormat::None;
		size_t _alignedSize = 0;
		string _filepath;
	private:
		virtual void allocate() = 0;
		virtual void release() = 0;
	public:
		Image() = delete;
		Image(const std::string_view& path) : _filepath(path) { }
		Image(const uint32& width, const uint32& height, const ImageFormat& format, void* data = nullptr)
			: _width(width), _height(height), _format(format) { }
		Image(Image&&) = delete;

		virtual void setData(void* data) = 0;
		virtual void resize(const uint32& width, const uint32& height) = 0;

		inline const uint32& getWidth() const { return _width; }
		inline const uint32& getHeight() const { return _height; }

		virtual ~Image() = default;
	};
}