#pragma once
#include "Timestep.h"
#include <InputCore/events/Event.h>

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

		FORCE_INLINE const string& getName() const { return m_debugName; }

		virtual ~Layer() = default;
	};
}