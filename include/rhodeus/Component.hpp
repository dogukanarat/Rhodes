#ifndef INCLUDED_RHODEUS_ABSTRACT_COMPONENT_HPP
#define INCLUDED_RHODEUS_ABSTRACT_COMPONENT_HPP

#include <string>
#include "rhodeus/Common.hpp"

namespace Rhodeus
{
    class AbstractComponent
    {
    public:
        AbstractComponent(const std::string& name);
        virtual ~AbstractComponent();

        std::string name() const { return _name; }

        virtual int32_t initialize() = 0;
        virtual int32_t finalize() = 0;

    protected:
        void emit(const std::string& signalName);

    private:
        std::string _name;
    };

    class ComponentManager
    {
    public:
        static ComponentManager& getInstance()
        {
            static ComponentManager instance;
            return instance;
        }

        void registerComponent(AbstractComponent* component);
        void deregisterComponent(AbstractComponent* component);

    protected:
        ComponentManager() {}
        ComponentManager(ComponentManager const&) = delete;
        void operator=(ComponentManager const&) = delete;

    private:
        std::vector<AbstractComponent*> _components;
    };
}

#endif // INCLUDED_RHODEUS_ABSTRACT_COMPONENT_HPP
