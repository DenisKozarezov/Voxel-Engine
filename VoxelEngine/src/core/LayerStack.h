#pragma once
#include <deque>
#include "Layer.h"

namespace VoxelEngine::renderer
{
	class VOXEL_API LayerStack final
	{
	private:
		std::deque<Layer*> m_layers;
		uint32 m_layerInsertIndex = 0;
	public:
		LayerStack() = default;
		~LayerStack() = default;

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);
		void detach();
		void onUpdate(const Timestep& time);
		void onFixedUpdate(const Timestep& time);
		void onImGuiRender();
		void onEvent(input::Event& e);

		std::deque<Layer*>::iterator begin() { return m_layers.begin(); }
		std::deque<Layer*>::iterator end() { return m_layers.end(); }
		std::deque<Layer*>::reverse_iterator rbegin() { return m_layers.rbegin(); }
		std::deque<Layer*>::reverse_iterator rend() { return m_layers.rend(); }

		std::deque<Layer*>::const_iterator begin() const { return m_layers.begin(); }
		std::deque<Layer*>::const_iterator end()	const { return m_layers.end(); }
		std::deque<Layer*>::const_reverse_iterator rbegin() const { return m_layers.rbegin(); }
		std::deque<Layer*>::const_reverse_iterator rend() const { return m_layers.rend(); }
	};
}