#pragma once

#include <string>
namespace xlab {
class IPushEvent {
   public:
    IPushEvent() {}

    virtual ~IPushEvent() {}

    virtual void OnConnected() = 0;

    virtual void OnReconnecting(int times) = 0;

    virtual void OnDisconnect() = 0;

    virtual void OnStatEvent(int64_t delayMs, int64_t netBand) = 0;
};

}  // namespace xlab