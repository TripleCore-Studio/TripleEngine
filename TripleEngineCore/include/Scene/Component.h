#ifndef COMPONENT_H
#define COMPONENT_H

namespace TripleEngineCore::Scene {

    class SceneObject;

    class Component {
    public:
        SceneObject* owner = nullptr;

        virtual ~Component() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(float dt) {}
    };
}

#endif // COMPONENT_H
