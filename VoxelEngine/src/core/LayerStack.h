#pragma once
#include <deque>
#include <core/PrimitiveTypes.h>
#include "Layer.h"

namespace VoxelEngine::renderer
{
	class VOXEL_API LayerStack final
	{
	private:
		std::deque<Layer*> _layers;
		uint32 _layerInsertIndex = 0;
	public:
		LayerStack() = default;
		~LayerStack() = default;

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);
		void detach();
		void onUpdate(const Timestep& time);
		void onImGuiRender();

		std::deque<Layer*>::iterator begin() { return _layers.begin(); }
		std::deque<Layer*>::iterator end() { return _layers.end(); }
		std::deque<Layer*>::reverse_iterator rbegin() { return _layers.rbegin(); }
		std::deque<Layer*>::reverse_iterator rend() { return _layers.rend(); }

		std::deque<Layer*>::const_iterator begin() const { return _layers.begin(); }
		std::deque<Layer*>::const_iterator end()	const { return _layers.end(); }
		std::deque<Layer*>::const_reverse_iterator rbegin() const { return _layers.rbegin(); }
		std::deque<Layer*>::const_reverse_iterator rend() const { return _layers.rend(); }
	};
}