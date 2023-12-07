#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
	class PrimitivesPanel : public ImGuiWindow
	{
	public:
		PrimitivesPanel(const string& title);
		~PrimitivesPanel() noexcept = default;
		PrimitivesPanel(PrimitivesPanel const&) noexcept = delete;
		PrimitivesPanel(PrimitivesPanel&&) noexcept = delete;
		PrimitivesPanel& operator=(PrimitivesPanel const& rhs) noexcept = delete;
		PrimitivesPanel& operator=(PrimitivesPanel&& rhs) noexcept = delete;

		void onImGuiRender() override;
		void update(const Timestep& ts);
	};
}