#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	class ImGuiWindow
	{
	private:
		bool m_visible = true;
		bool m_hovered = false;
		float m_minSize[2] = { 200.0f, 400.0f };
		float m_maxSize[2] = { 99999.0f, 99999.0f };
		string m_title;
	public:
		ImGuiWindow(const string& title = "Window");

		INLINE const bool& isVisible() const { return m_visible; }
		INLINE const bool& isHovered() const { return m_hovered; }
		INLINE const string& title() const { return m_title; }
		INLINE virtual const ImGuiWindowFlags& flags() const { return 0; }
		virtual const bool hasToolbar() const { return false; }
		virtual const bool wantCaptureKeyboard() const { return false; }
		virtual const bool wantCaptureMouse() const { return false; }

		const bool begin();
		void end();
		void setVisibility(const bool& isVisible);
		void setMinSize(const float& minWidth, const float& minHeight);
		void show();
		void hide();

		virtual void onImGuiRender() = 0;
		virtual void onBegin() { }
		virtual void onEnd() { }
		virtual void onToolbar(const bool& hovered) { }
	};
}