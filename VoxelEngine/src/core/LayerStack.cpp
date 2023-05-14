#include "LayerStack.h"

namespace VoxelEngine::renderer
{
	LayerStack::~LayerStack()
	{
		for (Layer* layer : _layers)
		{
			layer->onDetach();
			delete layer;
		}
	}

	void LayerStack::pushLayer(const SharedRef<Layer>& layer)
	{
		_layers.emplace(_layers.begin() + _layerInsertIndex, layer.get());
		_layerInsertIndex++;
	}

	void LayerStack::pushOverlay(const SharedRef<Layer>& overlay)
	{
		_layers.emplace_back(overlay.get());
	}

	void LayerStack::popLayer(const SharedRef<Layer>& layer)
	{
		auto it = std::find(_layers.begin(), _layers.begin() + _layerInsertIndex, layer.get());
		if (it != _layers.begin() + _layerInsertIndex)
		{
			layer->onDetach();
			_layers.erase(it);
			_layerInsertIndex--;
		}
	}

	void LayerStack::popOverlay(const SharedRef<Layer>& overlay)
	{
		auto it = std::find(_layers.begin() + _layerInsertIndex, _layers.end(), overlay.get());
		if (it != _layers.end())
		{
			overlay->onDetach();
			_layers.erase(it);
		}
	}
	void LayerStack::onUpdate(const Timestep& time)
	{
		for (auto it = begin(); it != end(); ++it)
		{
			(*it)->onUpdate(time);
		}
	}
}