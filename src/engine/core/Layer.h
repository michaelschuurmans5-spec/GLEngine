#pragma once 

#include <string>

class Layer {
public:
	Layer(const std::string& name = "Layer") : m_DebugName(name) {}
	virtual ~Layer() = default;


	// LIFECYCLE METHODS 
	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float delaTime) {}
	virtual void OnRender() {}

	// GETTER
	const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;
};