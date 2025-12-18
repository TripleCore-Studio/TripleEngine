#ifndef ISYSTEM_H
#define ISYSTEM_H

namespace TripleEngineCore {
    enum class SystemPriority {
        Early = 0,
        Normal = 1,
        Late = 2 
    };

    class ISystem {
    public:
        virtual ~ISystem() = default;
        virtual void update(float dt) = 0;
        virtual void init() {}
        virtual void shutdown() {}
        virtual SystemPriority priority() const { return SystemPriority::Normal; }
    };
}

#endif // ISYSTEM_H
