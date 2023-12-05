#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	class PrimitivesPanel final
	{
	public:
		PrimitivesPanel();
		~PrimitivesPanel() noexcept = default;
		PrimitivesPanel(PrimitivesPanel const&) noexcept = delete;
		PrimitivesPanel(PrimitivesPanel&&) noexcept = delete;
		PrimitivesPanel& operator=(PrimitivesPanel const& rhs) noexcept = delete;
		PrimitivesPanel& operator=(PrimitivesPanel&& rhs) noexcept = delete;

		void onImGuiRender();
		void update(const Timestep& ts);
	};
}