#pragma once

#include <string>

namespace xlab
{
    class IEnctrl
    {
    public:
        IEnctrl() {}

        virtual ~IEnctrl() {}

        virtual void OnSetVideoParams(int width, int height, int kbitrate, int framerate, int gop) = 0;

        virtual void OnRequestKeyFrame() = 0;
    };

}