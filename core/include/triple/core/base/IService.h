#ifndef ISERVICE_SYSTEM_H
#define ISERVICE_SYSTEM_H

namespace triple::core {
    class IService {
    public:
        virtual ~IService() = default;
        virtual void init() {}
        virtual void shutdown() {}
    };
}

#endif // ISERVICE_SYSTEM_H
