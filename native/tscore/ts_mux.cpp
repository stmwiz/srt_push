#include "ts_mux.hpp"

namespace xlab
{

    TSMux::TSMux(TStream::OutputCallback outCb)
    {
        TStream::Param tsParam = {
            .audio_stream_id = 0xc0,
            .video_stream_id = 0xe0,
            .pmt_pid = 0x1000,
            .audio_pid = 0x101,
            .video_pid = 0x100,
            .pcr = 10,
            .program_list = {
                {.program_num = 0x01, .program_map_pid = 0x1000},
            },
            .output = outCb,

        };

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