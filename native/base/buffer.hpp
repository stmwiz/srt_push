#pragma once

#include <memory>
#include <cstring>

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
      offset_ = 0;
    }

    ~Buffer()
    {
      delete start_;
    }

    bool isNull() const
    {
      return len_ <= 0 || start_ == nullptr;
    }

    uint8_t *data() const
    {
      return start_;
    }

    const int64_t offset() const
    {
      return offset_;
    }

    const int64_t offset(int64_t offset_value)
    {
      if (offset_value >= len() || offset_value < 0)
      {
        return -1;
      }

      if (offset_value >= 0)
      {
        offset_ = offset_value;
      }

      return offset_;
    }

    const int64_t len() const
    {
      return len_;
    }

  private:
    uint8_t *start_ = nullptr;
    int64_t offset_ = 0;
    int64_t len_ = 0;
  };

  class SPBuffer final
  {
  public:
    static inline SPBuffer nullVal()
    {
      static SPBuffer bf = SPBuffer();
      return bf;
    }

  public:
    explicit SPBuffer(int len = 0, uint8_t *data = nullptr)
    {
      if (len == 0)
      {
        return;
      }

      spbuf_ = std::make_shared<Buffer>(len, data);
      if (spbuf_ == nullptr || spbuf_->isNull())
      {
        spbuf_ = nullptr;
      }
    }

    ~SPBuffer()
    {
      spbuf_ = nullptr;
    }

    bool isNull() const
    {
      return spbuf_ == nullptr;
    }

    const int64_t offset() const
    {
      if (isNull())
      {
        return -1;
      }

      return spbuf_->offset();
    }

    int64_t offset(int64_t offset_value)
    {
      if (isNull())
      {
        return -1;
      }

      return spbuf_->offset(offset_value);
    }

    const int64_t surplus() const
    {
      if (isNull())
      {
        return -1;
      }

      return len() - offset();
    }

    uint8_t *offsetData() const
    {

      if (isNull())
      {
        return nullptr;
      }

      return &data()[offset()];
    }

    uint8_t *offsetData(uint8_t *offset_ptr)
    {

      if (isNull() || offset_ptr == nullptr)
      {
        return nullptr;
      }

      if (offset(offset_ptr - data()) < 0)
      {
        return nullptr;
      }

      return &data()[offset()];
    }

    uint8_t *data() const
    {
      if (isNull())
      {
        return nullptr;
      }

      return spbuf_->data();
    }

    const int len() const
    {
      if (isNull())
      {
        return 0;
      }

      return spbuf_->len();
    }

    SPBuffer dump()
    {
      if (isNull())
      {
        return nullVal();
      }

      return SPBuffer(spbuf_->len(), spbuf_->data());
    }

    SPBuffer dump(int len)
    {
      if (isNull())
      {
        return nullVal();
      }

      if (len <= spbuf_->len())
      {
        return SPBuffer(len, spbuf_->data());
      }

      auto buf = std::make_shared<Buffer>(len);
      memcpy(buf->data(), spbuf_->data(), spbuf_->len());
      memset(&buf->data()[spbuf_->len()], 0, len - spbuf_->len());
      return SPBuffer(buf->len(), buf->data());
    }

    SPBuffer frontInsert(int len, uint8_t *data)
    {
      if (len <= 0 || data == nullptr)
      {
        return nullVal();
      }

      if (isNull())
      {
        return SPBuffer(len, data);
      }

      SPBuffer buf(spbuf_->len() + len);
      memcpy(buf.data(), spbuf_->data(), spbuf_->len());
      memcpy(&buf.data()[spbuf_->len()], data, len);

      return buf;
    }

    SPBuffer backInsert(int len, uint8_t *data)
    {
      if (len <= 0 || data == nullptr)
      {
        return nullVal();
      }

      if (isNull())
      {
        return SPBuffer(len, data);
      }

      SPBuffer buf(len + spbuf_->len());
      memcpy(buf.data(), data, len);
      memcpy(&buf.data()[len], spbuf_->data(), spbuf_->len());

      return buf;
    }

  private:
    std::shared_ptr<Buffer> spbuf_ = nullptr;
  };
}