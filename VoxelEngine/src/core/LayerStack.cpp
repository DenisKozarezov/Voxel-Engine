#include "LayerStack.h"

namespace VoxelEngine::renderer
{
	void LayerStack::pushLayer(Layer* layer)
	{
		_layers.emplace(_layers.begin() + _layerInsertIndex, layer);
		_layerInsertIndex++;
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		_layers.emplace_back(overlay);
	}

	void LayerStack::popLayer(Layer* layer)
	{
		auto it = std::find(_layers.begin(), _layers.begin() + _layerInsertIndex, layer);
		if (it != _layers.begin() + _layerInsertIndex)
		{
			layer->onDetach();
			_layers.erase(it);
			_layerInsertIndex--;
		}
	}

	void LayerStack::popOverlay(Layer* overlay)
	{
		auto it = std::find(_layers.begin() + _layerInsertIndex, _layers.end(), overlay);
		if (it != _layers.end())
		{
			overlay->onDetach();
			_layers.erase(it);
		}
	}
	void LayerStack::detach()
	{
		for (Layer* layer : _layers)
		{
			layer->onDetach();
			delete layer;
		}
	}
	void LayerStack::onUpdate(const Timestep& time)
	{
		for (auto it = begin(); it != end(); ++it)
		{
			(*it)->onUpdate(time);
		}
	}
	void LayerStack::onImGuiRender()
	{
		for (auto it = begin(); it != end(); ++it)
		{
			(*it)->onImGuiRender();
		}
	}
	void LayerStack::onEvent(input::Event& e)
	{
		for (auto it = begin(); it != end(); ++it)
		{
			(*it)->onEvent(e);
		}
	}
}