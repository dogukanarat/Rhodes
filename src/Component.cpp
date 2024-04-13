#include <iostream>

#include <../include/rhodeus/Component.hpp>

using namespace Rhodeus;

AbstractComponent::AbstractComponent(const std::string& name)
    : _name(name)
{
    ComponentManager::getInstance().registerComponent(this);
}

AbstractComponent::~AbstractComponent()
{
    ComponentManager::getInstance().deregisterComponent(this);
}

void AbstractComponent::emit(const std::string& signalName)
{
    LOGD << fmt::format(
        "Component `{}` emitted signal `{}`",
        _name,
        signalName
    );
}

void ComponentManager::registerComponent(AbstractComponent* component)
{
    // std::cout
    //     << fmt::format("Component `{}` registered", component->name())
    //     << std::endl;

    _components.push_back(component);
}
void ComponentManager::deregisterComponent(AbstractComponent* component)
{
    // std::cout
    //     << fmt::format("Component `{}` deregistered", component->name())
    //     << std::endl;

    _components.erase(
        std::remove(_components.begin(), _components.end(), component),
        _components.end()
    );
}
