#pragma once

#include <memory>

#include "buffer.hpp"

namespace xlab::base
{
    enum class PacketType
    {
        None = -1,
        NonKeyFrameVideo = 0,
        KeyFrameVideo = 1,
        Audio = 2,
    };

    struct Packet final
    {
        PacketType type = PacketType::None;
        std::shared_ptr<Buffer> head = nullptr;
        std::shared_ptr<Buffer> body = nullptr;
        int64_t dtsUs = 0;
        int rotate = 0;

        static Packet nullVal()
        {
            static Packet packet;
            return packet;
        }

        bool isNonKeyFrameVideo() const
        {
            return type == PacketType::NonKeyFrameVideo;
        }

        bool isKeyFrameVideo() const
        {
            return type == PacketType::KeyFrameVideo;
        }

        bool isAudio() const
        {
            return type == PacketType::Audio;
        }

        bool isNull()
        {
            return type == PacketType::None;
        }
    };

} // namespace xlab
