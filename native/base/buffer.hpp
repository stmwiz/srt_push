#pragma once

#include <memory>
#include <cstring>
#include <cassert>
namespace xlab::base
{

  class Buffer final
  {
  public:
    explicit Buffer(int64_t len, uint8_t *data = nullptr)
    {
      if (len <= 0)
      {
        return;
      }

      if (data == nullptr)
      {
        start_ = new uint8_t[len];
        ::memset(start_, 0, len);
        len_ = len;
        return;
      }

      start_ = new uint8_t[len];
      ::memcpy(start_, data, len);
      len_ = len;
      offsetIndex_ = 0;
    }

    ~Buffer()
    {
      delete[] start_;
    }

    bool isNull() const
    {
      return len_ <= 0 || start_ == nullptr;
    }

    uint8_t *start() const
    {
      return start_;
    }

    uint8_t *offset() const
    {
      return start() + offsetIndex();
    }

    const void setOffset(uint8_t *offset)
    {
      setOffsetIndex(offset - start());
    }

    const int64_t offsetIndex() const
    {
      return offsetIndex_;
    }

    const void setOffsetIndex(int64_t offset_value)
    {
      assert(offset_value >= 0);
      assert(offset_value < len());
      offsetIndex_ = offset_value;
    }

    const int64_t len() const
    {
      return len_;
    }

    const int64_t surplus() const
    {
      return len() - offsetIndex();
    }

  private:
    uint8_t *start_ = nullptr;
    int64_t offsetIndex_ = 0;
    int64_t len_ = 0;
  };
}