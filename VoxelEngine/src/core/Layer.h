#pragma once
#include "Timestep.h"
#include "input/events/Event.h"

namespace VoxelEngine::renderer
{
	class Layer
	{
	private:
		string m_debugName;
	public:
		Layer() = default;
		Layer(const string& name = "Layer") : m_debugName(name) { }
		Layer(Layer const&) noexcept = delete;
		Layer(Layer&&) noexcept = delete;
		Layer& operator= (Layer const& rhs) noexcept = delete;
		Layer& operator= (Layer&& rhs) noexcept = delete;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(const Timestep& time) {}
		virtual void onFixedUpdate(const Timestep& time) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(input::Event& event) {}

		INLINE const std::string& getName() { return m_debugName; }

		virtual ~Layer() = default;
	};
}