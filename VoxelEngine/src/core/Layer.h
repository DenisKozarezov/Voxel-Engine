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
		Layer(const string& name = "Layer") : _debugName(name) { }

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(const Timestep& time) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(input::Event& event) {}

		inline const std::string& getName() const { return _debugName; }

		virtual ~Layer() = default;
	};
}