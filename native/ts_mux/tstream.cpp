
#include "tstream.hpp"
#include "crc.hpp"
#include "base/bitstream.hpp"

namespace xlab
{

    TStream::TStream(const Param &par) : param(par)
    {
        tsBuf = std::make_shared<base::Buffer>(TS_PACKET_SIZE);
    }

    TStream::~TStream()
    {
    }

    bool TStream::writeFrame(const base::Packet &frame)
    {
        header->payload_unit_start_indicator = 1;
        pes->payload_len = frame.body->len() + PES_HEADER_LEN;
        if (frame.isKeyFrameVideo())
        {
            pes->payload_len += frame.head->len();
        }

        uint16_t pid = 0;
        while (pes->payload_len > 0)
        {
            if (frame.isAudio())
            {
                pid = param.audio_pid;
                header->continuity_counter = audioWriteCounter++;
                if (audioWriteCounter == 16)
                {
                    audioWriteCounter = 0;
                }
            }
            else
            {
                pid = param.video_pid;
                header->continuity_counter = videoWriteCounter++;
                if (videoWriteCounter == 16)
                {
                    videoWriteCounter = 0;
                }
            }

            header->pid_hi = (pid >> 8) & 0x1f;
            header->pid_lo = (pid)&0xff;

            writePacket(pes, frame);
        }

        return true;
    }

    bool TStream::writePAT()
    {
        header->payload_unit_start_indicator = 1;
        header->adaptation_field_control = 0x01;
        return writePacket(pat);
    }

    bool TStream::writePMT()
    {
        header->pid_hi = (param.pmt_pid >> 8) & 0x1f;
        header->pid_lo = (param.pmt_pid) & 0xff;
        header->payload_unit_start_indicator = 1;
        header->adaptation_field_control = 0x01;
        return writePacket(pmt);
    }

    bool TStream::newTS()
    {
        header = std::make_shared<Header>();
        adp = std::make_shared<ADPField>();
        pat = std::make_shared<PAT>(param.program_list.size());
        pmt = std::make_shared<PMT>(param.program_list.size(), param.video_pid);
        pes = std::make_shared<PES>();

        return true;
    }

    void TStream::deleteTS()
    {
    }

    bool TStream::writePacket(std::shared_ptr<Packet> packet, const base::Packet &frame)
    {
        auto tmp_ptr = tsBuf->offset();
        if (packet->has_pcr)
        {
            header->adaptation_field_control = 0x03;
        }
        else if (packet->adp_filling && packet->payload_len)
        {
            if (TS_HEADER_LEN + packet->payload_len < tsBuf->len())
            {
                header->adaptation_field_control = 0x03;
            }
            else
            {
                header->adaptation_field_control = 0x01;
            }
        }
        else
        {
            header->adaptation_field_control = 0x01;
        }

        /* write ts header : 4bytes */
        *(Header *)tmp_ptr = *header;
        tmp_ptr += sizeof(*header);
        /* write adaption field */
        if (header->adaptation_field_control != 0x01)
        {
            // xlogt("check adaption field error");
            return false;
        }
        else if (header->adaptation_field_control == 0x03)
        {
            int adp_len = 1; // flags

            if (packet->has_pcr)
            {
                adp_len += PCR_LEN;
            }

            int stuff_len = tsBuf->len() - (TS_HEADER_LEN + 1 + adp_len + packet->payload_len); // 1 - adaption_field_length
            if (stuff_len > 0)
            {
                adp_len += stuff_len;
            }

            adp->adaption_field_length = adp_len;
            if (packet->has_pcr)
            {
                adp->PCR_flag = 0x01;
                adp->random_access_indicator = 0x01;
            }
            else
            {
                adp->PCR_flag = 0x00;
                adp->random_access_indicator = 0x00;
            }

            *(ADPField *)tmp_ptr = *adp;
            tmp_ptr += sizeof(ADPField);

            if (packet->has_pcr)
            {
                int64_t pcr_base = param.pcr % 8589934592;
                // (pcr/90000 * 27000000) % 300
                int64_t pcr_ext = (param.pcr * 300) % 300;

                *tmp_ptr++ = pcr_base >> 25;
                *tmp_ptr++ = pcr_base >> 17;
                *tmp_ptr++ = pcr_base >> 9;
                *tmp_ptr++ = pcr_base >> 1;
                *tmp_ptr++ = pcr_base << 7 | pcr_ext >> 8 | 0x7e;
                *tmp_ptr++ = pcr_ext;
            }

            if (stuff_len > 0)
            {
                memset(tmp_ptr, 0xff, stuff_len);
                tmp_ptr += stuff_len;
            }
        }

        /* write pointer field : 1byte */
        if (header->payload_unit_start_indicator && packet->has_pointer_field)
        {
            *tmp_ptr++ = 0x00;
        }

        tsBuf->setOffset(tmp_ptr);

        /* write payload */
        switch (packet->type())
        {
        case Packet::Type::PAT:
        {
            PAT::writePayload(tsBuf, param.program_list);
            break;
        }

        case Packet::Type::PMT:
        {
            PMT::writePayload(tsBuf, param.program_list.size(), param.video_pid, param.audio_pid);
            break;
        }

        case Packet::Type::PES:
        {
            auto &streamid = frame.isAudio() ? param.audio_pid : param.video_pid;
            PES::writePayload(tsBuf, packet->payload_len, header, streamid, frame);
            break;
        }
        default:
        {
            break;
        }
        }

        /* write stuff */
        if (!packet->adp_filling && (tsBuf->surplus() > 0))
        {
            memset(tsBuf->offset(), 0xff, tsBuf->surplus());
        }

        tsBuf->setOffsetIndex(0);
        if (param.output != nullptr)
        {
            param.output(tsBuf);
        }

        return true;
    }

    bool TStream::PAT::writePayload(std::shared_ptr<base::Buffer> &buf, std::vector<Program> programList)
    {
        auto tmp_ptr = buf->offset();
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

        const auto start = buf->start() + 5;
        const auto crc = Crc::CalcCrc(start, tmp_ptr - start);
        base::Bitstream::WB32<decltype(tmp_ptr)>(tmp_ptr, crc);
        tmp_ptr += 4;

        buf->setOffset(tmp_ptr);
        return true;
    }

    bool TStream::PMT::writePayload(std::shared_ptr<base::Buffer> &buf, int program_num, int video_pid, int audio_pid)
    {
        auto tmp_ptr = buf->offset();
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

        const auto start = buf->start() + 5;
        const auto crc = Crc::CalcCrc(start, tmp_ptr - start);
        base::Bitstream::WB32<decltype(tmp_ptr)>(tmp_ptr, crc);
        tmp_ptr += 4;

        buf->setOffset(tmp_ptr);

        return true;
    }

    bool TStream::PES::writePayload(std::shared_ptr<base::Buffer> &buf,
                                    int &payload_len,
                                    std::shared_ptr<Header> &header,
                                    uint8_t stream_id,
                                    const base::Packet &frame)
    {
        auto tmp_ptr = buf->offset();
        const uint16_t pes_pkt_len = payload_len - 4 - 2; // 4 packet start code + stream id
        const int64_t pts = frame.dtsUs * 90 / 1000;
        if (header->payload_unit_start_indicator)
        {
            *tmp_ptr++ = 0x00;
            *tmp_ptr++ = 0x00;
            *tmp_ptr++ = 0x01;
            /* stream id */
            *tmp_ptr++ = stream_id;
            *tmp_ptr++ = pes_pkt_len >> 8;
            *tmp_ptr++ = pes_pkt_len & 0xff;
            /* optional pes header */
            *(PES *)tmp_ptr = PES();
            tmp_ptr += sizeof(PES);
            /* pts */
            *tmp_ptr++ = (0x02 << 4) | (pts >> 29 & 0xe) | 0x01;
            *tmp_ptr++ = pts >> 22 & 0xff;
            *tmp_ptr++ = ((pts >> 14) & 0xfe) | 0x01;
            *tmp_ptr++ = (pts >> 7) & 0xff;
            *tmp_ptr++ = ((pts << 1) & 0xfe) | 0x01;

            /* aud */
            if (frame.isKeyFrameVideo())
            {
                memcpy(tmp_ptr, frame.head->start(), frame.head->len());
                tmp_ptr += frame.head->len();
            }

            if (header->adaptation_field_control == 0x03)
            {
                payload_len -= (tmp_ptr - buf->offset());
            }

            header->payload_unit_start_indicator = 0;
        }

        const int len = buf->len() - (tmp_ptr - buf->start());
        memcpy(tmp_ptr, frame.body->offset(), len);
        tmp_ptr += len;

        if (header->adaptation_field_control != 0x03)
        {
            payload_len -= 184;
        }
        else
        {
            payload_len -= len;
        }

        buf->setOffset(tmp_ptr);

        return true;
    }
}