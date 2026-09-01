#pragma once 

#include "Layer.h"
#include <vector>


class LayerStack {
public:
	LayerStack() = default;
	~LayerStack();


	void PushLayer(Layer* layer);
	void PopLayer(Layer* layer);

	// ITERATORS TO ALLOW RANGE BASED FOR LOOPS INSIDE APPLICATION
	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	std::vector<Layer*> m_Layers;
};