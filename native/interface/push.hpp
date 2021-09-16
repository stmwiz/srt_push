#pragma once

#include <string.h>
namespace xlab
{
    class IPush
    {
    public:
        IPush(){};
        virtual ~IPush(){};

        void write(uint8_t *data) ;

    };
}
