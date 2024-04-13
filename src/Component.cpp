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

    mComponents.push_back(component);
}

void ComponentManager::deregisterComponent(AbstractComponent* component)
{
    // std::cout
    //     << fmt::format("Component `{}` deregistered", component->name())
    //     << std::endl;

    mComponents.erase(
        std::remove(mComponents.begin(), mComponents.end(), component),
        mComponents.end()
    );
}

void ComponentManager::initializeComponents()
{
    for (auto component : mComponents)
    {
        component->initialize();
    }
}

void ComponentManager::destroyComponents()
{
    for (auto component : mComponents)
    {
        component->finalize();
    }
}
