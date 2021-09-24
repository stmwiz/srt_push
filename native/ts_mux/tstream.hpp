#pragma once

#include <memory>
#include <functional>

#include "base/buffer.hpp"
#include "base/packet.hpp"
namespace xlab
{
    class TStream final
    {
    public:
        struct Program
        {
            int program_num = 0;
            int program_map_pid = 0;
        };

        struct Param
        {
            uint8_t audio_stream_id = 0;
            uint8_t video_stream_id = 0;
            uint16_t pmt_pid = 0;
            uint16_t audio_pid = 0;
            uint16_t video_pid = 0;
            int64_t pcr = 0;
            std::vector<Program> program_list;
            std::function<int(std::shared_ptr<base::Buffer> buf)> output;
        };

        struct Header
        {
            explicit Header()
            {
                ::memset((void *)(this), 0, sizeof(*this));
                this->sync_byte = 0x47;
            }

#ifdef WORDS_BIGENDIAN
            uint8_t sync_byte : 8; /* always be 0x47 */
            uint8_t transport_error_indicator : 1;
            uint8_t payload_unit_start_indicator : 1; /* if a PES-packet starts in the TS-packet */
            uint8_t transport_priority : 1;           /* meanless to IRD, can be ignored */
            uint8_t pid_hi : 5;
            uint8_t pid_lo : 8;
            uint8_t transport_scrambling_control : 2; /* 00: no scramble, 01: reserved,
                                                           10: even key scrambled, 11: odd key scrambled */
            uint8_t adaptation_field_control : 2;     /* 00: reserved
                                                           01: no adaptation field, payload only
                                                           10: adaptation field only, no payload
                                                           11: adaptation field followed by payload */
            uint8_t continuity_counter : 4;
#else
            uint8_t sync_byte : 8;
            uint8_t pid_hi : 5;
            uint8_t transport_priority : 1;
            uint8_t payload_unit_start_indicator : 1;
            uint8_t transport_error_indicator : 1;
            uint8_t pid_lo : 8;
            uint8_t continuity_counter : 4;
            uint8_t adaptation_field_control : 2;
            uint8_t transport_scrambling_control : 2;
#endif
        };

        struct ADPField
        {
            explicit ADPField()
            {
                ::memset((void *)(this), 0, sizeof(*this));
            }

            uint8_t adaption_field_length : 8;
            uint8_t adaption_file_extension_flag : 1;
            uint8_t transport_private_data_flag : 1;
            uint8_t splicing_point_flag : 1;
            uint8_t OPCR_flag : 1;
            uint8_t PCR_flag : 1;
            uint8_t elementary_stream_priority_indicator : 1;
            uint8_t random_access_indicator : 1;
            uint8_t discontinuity_indicator : 1;
        };

        struct Packet
        {
            enum class Type
            {
                NONE,
                PAT,
                PMT,
                PES,
            };

            uint8_t has_pointer_field = 0;
            /*
     * pat & pmt : filling 0xff at the end, don't use adaption field
     * pes       : use adaption field to filling if 188 bytes not enough
     */
            uint8_t adp_filling = 0;
            int payload_len = 0;
            uint8_t has_pcr = 0;

            virtual const Type type() const
            {
                return Type::NONE;
            }
        };

        struct PAT : Packet
        {
            static bool writePayload(std::shared_ptr<base::Buffer> &buf, std::vector<Program> programList);

            explicit PAT()
            {
                ::memset((void *)(this), 0, sizeof(*this));
            }

            explicit PAT(uint8_t program_count)
            {
                ::memset((void *)(this), 0, sizeof(*this));
                this->has_pointer_field = 1;
                this->has_pcr = 0;
                this->adp_filling = 0;

                uint16_t section_length = 5 + program_count * 4 + 4; // start + n loop + crc
                this->table_id = 0x00;
                this->section_syntax_indicator = 1;
                this->transport_stream_id_hi = 0x00;
                this->transport_stream_id_lo = 0x01;
                this->section_length_hi = (section_length >> 8) & 0x0f;
                this->section_length_lo = section_length & 0xff;
                this->reversed_2 = 0x3;
                this->current_next_indicator = 1;
            }

            virtual const Type type() const
            {
                return Type::PAT;
            }

#ifdef WORDS_BIGENDIAN
            uint8_t table_id : 8;
            uint8_t section_syntax_indicator : 1;
            uint8_t : 3;
            uint8_t section_length_hi : 4;
            uint8_t section_length_lo : 8;
            uint8_t transport_stream_id_hi : 8;
            uint8_t transport_stream_id_lo : 8;
            uint8_t reversed_2 : 2;
            uint8_t version_number : 5;
            uint8_t current_next_indicator : 1;
            uint8_t section_number : 8;
            uint8_t last_section_number : 8;
#else
            uint8_t table_id : 8;
            uint8_t section_length_hi : 4;
            uint8_t : 3;
            uint8_t section_syntax_indicator : 1;
            uint8_t section_length_lo : 8;
            uint8_t transport_stream_id_hi : 8;
            uint8_t transport_stream_id_lo : 8;
            uint8_t current_next_indicator : 1;
            uint8_t version_number : 5;
            uint8_t reversed_2 : 2;
            uint8_t section_number : 8;
            uint8_t last_section_number : 8;
#endif
        };

        struct PMT : Packet
        {
            static bool writePayload(std::shared_ptr<base::Buffer> &buf, int program_num, int video_pid, int audio_pid);

            explicit PMT()
            {
                ::memset((void *)(this), 0, sizeof(*this));
            }

            explicit PMT(int program_num, int video_pid)
            {
                ::memset((void *)(this), 0, sizeof(*this));
                this->has_pointer_field = 1;
                this->has_pcr = 0;
                this->adp_filling = 0;

                const int section_length = 9 + 2 * 5 + 4;
                this->table_id = 0x02;
                this->section_syntax_indicator = 1;
                this->program_number_hi = program_num >> 8;
                this->program_number_lo = program_num & 0xff;
                this->reversed_2 = 0x3;
                this->current_next_indicator = 1;
                this->reversed_3 = 0x7;
                this->pcr_pid_hi = (video_pid >> 8) & 0x1f;
                this->pcr_pid_lo = video_pid & 0xff;
                this->reversed_4 = 0xf;
                this->section_length_hi = (section_length >> 8) & 0x0f;
                this->section_length_lo = section_length & 0xff;
            }

            virtual const Type type() const
            {
                return Type::PMT;
            }

#ifdef WORDS_BIGENDIAN
            uint8_t table_id : 8;
            uint8_t section_syntax_indicator : 1;
            uint8_t : 3;
            uint8_t section_length_hi : 4;
            uint8_t section_length_lo : 8;
            uint8_t program_number_hi : 8;
            uint8_t program_number_lo : 8;
            uint8_t reversed_2 : 2;
            uint8_t version_number : 5;
            uint8_t current_next_indicator : 1;
            uint8_t section_number : 8;
            uint8_t last_section_number : 8;
            uint8_t reversed_3 : 3;
            uint8_t pcr_pid_hi : 5;
            uint8_t pcr_pid_lo : 8;
            uint8_t reversed_4 : 4;
            uint8_t program_info_length_hi : 4;
            uint8_t program_info_length_lo : 8;
#else
            uint8_t table_id : 8;
            uint8_t section_length_hi : 4;
            uint8_t : 3;
            uint8_t section_syntax_indicator : 1;
            uint8_t section_length_lo : 8;
            uint8_t program_number_hi : 8;
            uint8_t program_number_lo : 8;
            uint8_t current_next_indicator : 1;
            uint8_t version_number : 5;
            uint8_t reversed_2 : 2;
            uint8_t section_number : 8;
            uint8_t last_section_number : 8;
            uint8_t pcr_pid_hi : 5;
            uint8_t reversed_3 : 3;
            uint8_t pcr_pid_lo : 8;
            uint8_t program_info_length_hi : 4;
            uint8_t reversed_4 : 4;
            uint8_t program_info_length_lo : 8;
#endif
        };

        struct PES : Packet
        {
            static bool writePayload(std::shared_ptr<base::Buffer> &buf,
                                     int &payload_len,
                                     std::shared_ptr<Header> &header,
                                     std::shared_ptr<base::Packet> &frame,
                                     uint8_t stream_id);

            explicit PES()
            {
                ::memset((void *)(this), 0, sizeof(*this));
                this->has_pointer_field = 0;
                this->has_pcr = 1;
                this->adp_filling = 1;

                this->PTS_DTS_flag = 0x02;
                this->reversed = 0x02;
                this->PES_header_data_length = 0x05; // pts length
            }

            virtual const Type type() const
            {
                return Type::PES;
            }

#ifdef WORDS_BIGENDIAN
            uint8_t : 2;
            uint8_t PES_scrambling_control : 2;
            uint8_t PES_priority : 1;
            uint8_t data_alignment_indicator : 1;
            uint8_t copyright : 1;
            uint8_t original_or_copy : 1;
            uint8_t PTS_DTS_flag : 2;
            uint8_t ESCR_flag : 1;
            uint8_t ES_rate_flag : 1;
            uint8_t DSM_trick_mode_flag : 1;
            uint8_t additional_copyright_info_flag : 1;
            uint8_t PES_crc_flag : 1;
            uint8_t PES_extension_flag : 1;
            uint8_t PES_header_data_length : 8;
#else
            uint8_t original_or_copy : 1;
            uint8_t copyright : 1;
            uint8_t PES_priority : 1;
            uint8_t data_alignment_indicator : 1;
            uint8_t PES_scrambling_control : 2;
            uint8_t reversed : 2;
            uint8_t PES_extension_flag : 1;
            uint8_t PES_crc_flag : 1;
            uint8_t additional_copyright_info_flag : 1;
            uint8_t DSM_trick_mode_flag : 1;
            uint8_t ES_rate_flag : 1;
            uint8_t ESCR_flag : 1;
            uint8_t PTS_DTS_flag : 2;
            uint8_t PES_header_data_length : 8;
#endif
        };

    public:
        explicit TStream() = delete;
        explicit TStream(const Param &par);
        ~TStream();

    public:
        bool writeFrame(std::shared_ptr<base::Packet> frame);

    private:
        bool writePAT();

        bool writePMT();

    private:
        bool newTS();

        void deleteTS();

        bool writePacket(std::shared_ptr<Packet> packet, std::shared_ptr<base::Packet> frame = nullptr);

    public:
        static constexpr uint8_t PAT_SECT_HEADER_LEN = 8;
        static constexpr uint8_t PMT_SECT_HEADER_LEN = 12;
        static constexpr uint8_t TS_HEADER_LEN = 4;
        static constexpr uint8_t PES_HEADER_LEN = (6 + 3 + 5);
        static constexpr uint8_t PCR_LEN = 6;
        static constexpr uint8_t PTS_LEN = 5;
        static constexpr uint8_t OPTIONAL_PES_HEADER_LEN = 3;
        static constexpr uint8_t TS_PACKET_SIZE = 188;

    private:
        static inline std::vector<uint8_t> h264_aud = {0x00, 0x00, 0x00, 0x01, 0x09, 0xF0};

    private:
        Param param;
        std::shared_ptr<base::Buffer> tsBuf = nullptr;
        std::shared_ptr<Header> header = nullptr;
        std::shared_ptr<ADPField> adp = nullptr;

        std::shared_ptr<PAT> pat = nullptr;
        std::shared_ptr<PMT> pmt = nullptr;
        std::shared_ptr<PES> pes = nullptr;

        uint8_t audioWriteCounter = 0;
        uint8_t videoWriteCounter = 0;
    };

}