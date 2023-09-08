#include <iostream>
#include "rhodeus/Component.hpp"

using namespace Rhodeus;

AbstractComponent::AbstractComponent(std::string name)
    : m_name(name)
{
    ComponentManager::getInstance().registerComponent(this);
}

AbstractComponent::~AbstractComponent()
{
    ComponentManager::getInstance().deregisterComponent(this);
}

void AbstractComponent::emit(const std::string& signalName)
{
    LOGD << fmt::format("Component `{}` emitted signal `{}`", m_name, signalName);
}

void ComponentManager::registerComponent(AbstractComponent* component)
{
    std::cout << fmt::format("Component `{}` registered", component->name()) << std::endl;
    m_components.push_back(component);
}
void ComponentManager::deregisterComponent(AbstractComponent* component)
{
    std::cout << fmt::format("Component `{}` deregistered", component->name()) << std::endl;
    m_components.erase(std::remove(m_components.begin(), m_components.end(), component), m_components.end());
}
