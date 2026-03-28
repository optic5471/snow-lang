
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <utility>

#include <util/Assertions.hpp>

template <typename T>
class ServiceLocator {
    static T* mService;

public:
    template <typename... tArgs>
    static void init(tArgs&&... args) {
        DEBUG_ASSERT(mService == nullptr, "Service has already been initialized");
        mService = new T(std::forward<tArgs>(args)...);
    }

    static T& fetch() {
        DEBUG_ASSERT(mService, "Service has not been initialized");
        return *mService;
    }

    static T* tryFetch() {
        return mService;
    }

    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator(ServiceLocator&&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
    ServiceLocator& operator=(ServiceLocator&&) = delete;
};
