#pragma once

#include <string>
namespace xlab
{
    class ICallBack
    {
        ICallBack() {}
        virtual ~ICallBack() {}

        virtual void OnConnected() = 0;

        virtual void OnReconnecting(int times) = 0;

        virtual void OnDisconnect() = 0;

        virtual void OnStatusEvent(int64_t delayMs, int64_t netBand) = 0;

        virtual void OnShowBitrate(int64_t valKbps) = 0;
    };

}