#ifndef ISERVICE_SYSTEM_H
#define ISERVICE_SYSTEM_H

namespace TripleEngineCore {
    class IServiceSystem {
    public:
        virtual ~IServiceSystem() = default;
        virtual void init() {}
        virtual void shutdown() {}
    };
}

#endif // ISERVICE_SYSTEM_H
