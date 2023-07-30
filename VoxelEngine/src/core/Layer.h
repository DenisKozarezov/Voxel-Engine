#pragma once
#include "Timestep.h"
#include "input/events/Event.h"

namespace VoxelEngine::renderer
{
	class Layer
	{
	private:
		string _debugName;
	public:
		Layer() = default;
		Layer(const string& name = "Layer") : _debugName(name) { }
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

		inline const std::string& getName() const & { return _debugName; }

		virtual ~Layer() = default;
	};
}