#pragma once

#include <cmath>

namespace xlab::AACParse
{
    static int aacfreq[] = {96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350};

    struct LinkADTSFixheader
    {
        unsigned short syncword : 12;
        unsigned char id : 1;
        unsigned char layer : 2;
        unsigned char protection_absent : 1;
        unsigned char profile : 2;
        unsigned char sampling_frequency_index : 4;
        unsigned char private_bit : 1;
        unsigned char channel_configuration : 3;
        unsigned char original_copy : 1;
        unsigned char home : 1;
    };

    struct LinkADTSVariableHeader
    {
        unsigned char copyright_identification_bit : 1;
        unsigned char copyright_identification_start : 1;
        unsigned short aac_frame_length : 13;
        unsigned short adts_buffer_fullness : 11;
        unsigned char number_of_raw_data_blocks_in_frame : 2;
    };

    static inline void LinkParseAdtsfixedHeader(const unsigned char *pData, LinkADTSFixheader *_pHeader)
    {
        unsigned long long adts = 0;
        const unsigned char *p = pData;
        adts |= *p++;
        adts <<= 8;
        adts |= *p++;
        adts <<= 8;
        adts |= *p++;
        adts <<= 8;
        adts |= *p++;
        adts <<= 8;
        adts |= *p++;
        adts <<= 8;
        adts |= *p++;
        adts <<= 8;
        adts |= *p++;

        _pHeader->syncword = (adts >> 44);
        _pHeader->id = (adts >> 43) & 0x01;
        _pHeader->layer = (adts >> 41) & 0x03;
        _pHeader->protection_absent = (adts >> 40) & 0x01;
        _pHeader->profile = (adts >> 38) & 0x03;
        _pHeader->sampling_frequency_index = (adts >> 34) & 0x0f;
        _pHeader->private_bit = (adts >> 33) & 0x01;
        _pHeader->channel_configuration = (adts >> 30) & 0x07;
        _pHeader->original_copy = (adts >> 29) & 0x01;
        _pHeader->home = (adts >> 28) & 0x01;
    }

    static inline void LinkParseAdtsVariableHeader(const unsigned char *pData, LinkADTSVariableHeader *_pHeader)
    {
        unsigned long long adts = 0;
        adts = pData[0];
        adts <<= 8;
        adts |= pData[1];
        adts <<= 8;
        adts |= pData[2];
        adts <<= 8;
        adts |= pData[3];
        adts <<= 8;
        adts |= pData[4];
        adts <<= 8;
        adts |= pData[5];
        adts <<= 8;
        adts |= pData[6];

        _pHeader->copyright_identification_bit = (adts >> 27) & 0x01;
        _pHeader->copyright_identification_start = (adts >> 26) & 0x01;
        _pHeader->aac_frame_length = (adts >> 13) & ((int)::pow(2, 14) - 1);
        _pHeader->adts_buffer_fullness = (adts >> 2) & ((int)::pow(2, 11) - 1);
        _pHeader->number_of_raw_data_blocks_in_frame = adts & 0x03;
    }

} // namespace xlab::AACParse
