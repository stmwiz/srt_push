
#include "tstream.hpp"
#include "crc.hpp"
#include "base/bitstream.hpp"

namespace xlab
{

    TStream::TStream(const Param &par) : param(par)
    {
    }

    TStream::~TStream()
    {
    }

    bool TStream::writeFrame(const FrameInfo &frame)
    {
        return false;
    }

    bool TStream::init()
    {

        return false;
    }

    void TStream::deInit()
    {
    }

    bool TStream::newTS()
    {
        return false;
    }

    bool TStream::PAT::writePayload(uint8_t *&offset, uint8_t *start, std::vector<Program> programList)
    {
        if (offset == nullptr || programList.empty())
        {
            return false;
        }

        auto tmp_ptr = offset;
        *(PAT *)tmp_ptr = PAT(programList.size());
        tmp_ptr += sizeof(PAT);

        for (const auto &p : programList)
        {
            if (!(p.program_map_pid < (1 << 14)))
            {
                return false;
            }

            *tmp_ptr++ = p.program_num >> 8 & 0xff;
            *tmp_ptr++ = p.program_num & 0xff;
            *tmp_ptr++ = (0x7 << 5) | ((p.program_map_pid >> 8) & 0x1f);
            *tmp_ptr++ = p.program_map_pid & 0xff;
        }

        const auto crc = Crc::CalcCrc(start + 5, tmp_ptr - start - 5);
        base::Bitstream::WB32<decltype(tmp_ptr)>(tmp_ptr, crc);
        tmp_ptr += 4;
        offset = tmp_ptr;

        return true;
    }

    bool TStream::PMT::writePayload(uint8_t *&offset, uint8_t *start, int program_num, int video_pid, int audio_pid)
    {
        if (offset == nullptr)
        {
            return false;
        }

        auto tmp_ptr = offset;
        *(PMT *)tmp_ptr = PMT(program_num, video_pid);
        tmp_ptr += sizeof(PMT);

        /* N loop */
        *tmp_ptr++ = 0x1b;                                   // stream_type : 1byte
        *tmp_ptr++ = (0x7 << 5) | ((video_pid >> 8) & 0x1f); // elementary_PID : high
        *tmp_ptr++ = video_pid & 0xff;                       // elementary_PID : low
        *tmp_ptr++ = 0xf0;                                   // es info length
        *tmp_ptr++ = 0x00;

        *tmp_ptr++ = 0x0f;                                   // stream_type : 1byte
        *tmp_ptr++ = (0x7 << 5) | ((audio_pid >> 8) & 0x1f); // elementary_PID : high
        *tmp_ptr++ = audio_pid & 0xff;                       // elementary_PID : low
        *tmp_ptr++ = 0xf0;                                   // es info length
        *tmp_ptr++ = 0x00;

        const auto crc = Crc::CalcCrc(start + 5, tmp_ptr - start - 5);
        base::Bitstream::WB32<decltype(tmp_ptr)>(tmp_ptr, crc);
        tmp_ptr += 4;
        offset = tmp_ptr;

        return true;
    }

}