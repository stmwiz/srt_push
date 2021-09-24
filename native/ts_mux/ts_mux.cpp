#include "ts_mux.hpp"

namespace xlab
{

    TSMux::TSMux(TStream::Param::Callback outCb)
    {
        TStream::Param tsParam{
            .audio_stream_id = 0xc0,
            .video_stream_id = 0xe0,
            .audio_pid = 0x101,
            .video_pid = 0x100,
            .pmt_pid = 0x1000,
            .pcr = 10,
            .output = outCb,
            .program_list = {
                {0x01, 0x1000},
            }};

        ts = std::make_shared<TStream>(tsParam);
    }

    TSMux::~TSMux()
    {
        ts = nullptr;
    }

    bool TSMux ::write(std::shared_ptr<base::Packet> frame)
    {
        return ts->writeFrame(frame);
    }

}