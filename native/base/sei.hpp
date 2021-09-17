#pragma once

#include <memory>
#include <vector>

#include "codec_id.hpp"

namespace xlab::base
{
    class SEI final
    {
    public:
        explicit SEI() {}

        ~SEI() {}

    public:
        ///自定义内容限定30bytes
        void update(CODEC_ID codecId, std::string value)
        {
            if (value.empty() || value.size() > 30 || (codecId == codecId_ && value == value_))
            {
                return;
            }

            decltype(AVCHeadData) headData;
            if (codecId == CODEC_ID::AVC)
            {
                headData = AVCHeadData;
            }
            else if (codecId == CODEC_ID::HEVC)
            {
                headData = HEVCHeadData;
            }
            else
            {
                return;
            }

            const uint8_t seiLen = UUIDData.size() + value.size() + EOFData.size();
            seiData_.clear();
            seiData_.insert(seiData_.end(), headData.begin(), headData.end());
            seiData_.push_back(seiLen);
            seiData_.insert(seiData_.end(), UUIDData.begin(), UUIDData.end());
            seiData_.insert(seiData_.end(), value.begin(), value.end());
            seiData_.insert(seiData_.end(), EOFData.begin(), EOFData.end());
            value_ = value;
            codecId_ = codecId;
        }

        const std::vector<uint8_t> &data() const
        {
            return seiData_;
        }

    private:
        static inline std::vector<uint8_t> AVCHeadData = {0x00, 0x00, 0x00, 0x01, 0x06, 0x05};
        static inline std::vector<uint8_t> HEVCHeadData = {0x00, 0x00, 0x00, 0x01, 0x4e, 0x01};
        static inline std::vector<uint8_t> UUIDData = {
            0x08, 0x6f, 0x36, 0x93,
            0xb7, 0xb3, 0x4f, 0x2c,
            0x96, 0x53, 0x21, 0x49,
            0x2f, 0xee, 0xe5, 0xb8};
        static inline std::vector<uint8_t> EOFData = {0x00};

    private:
        CODEC_ID codecId_ = CODEC_ID::NONE;
        std::string value_ = std::string();
        std::vector<uint8_t> seiData_{};
    };

}