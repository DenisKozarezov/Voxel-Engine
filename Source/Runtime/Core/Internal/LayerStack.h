#pragma once
#include <deque>
#include "Layer.h"

namespace VoxelEngine::renderer
{
	class LayerStack final
	{
	private:
		std::deque<Layer*> m_layers;
		uint32 m_layerInsertIndex = 0;
	public:
		LayerStack() noexcept = default;
		~LayerStack() noexcept = default;

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);
		void detach();
		void onUpdate(const Timestep& time);
		void onFixedUpdate(const Timestep& time);
		void onImGuiRender();
		void onEvent(input::Event& e);

		FORCE_INLINE std::deque<Layer*>::iterator begin() { return m_layers.begin(); }
		FORCE_INLINE std::deque<Layer*>::iterator end() { return m_layers.end(); }
		FORCE_INLINE std::deque<Layer*>::reverse_iterator rbegin() { return m_layers.rbegin(); }
		FORCE_INLINE std::deque<Layer*>::reverse_iterator rend() { return m_layers.rend(); }

		FORCE_INLINE std::deque<Layer*>::const_iterator begin() const { return m_layers.begin(); }
		FORCE_INLINE std::deque<Layer*>::const_iterator end()	const { return m_layers.end(); }
		FORCE_INLINE std::deque<Layer*>::const_reverse_iterator rbegin() const { return m_layers.rbegin(); }
		FORCE_INLINE std::deque<Layer*>::const_reverse_iterator rend() const { return m_layers.rend(); }
	};
}