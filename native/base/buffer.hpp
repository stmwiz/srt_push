#pragma once

#include <memory>
#include <cstring>

namespace xlab::base
{

  class Buffer final
  {
  public:
    explicit Buffer(int len, uint8_t *data = nullptr)
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

    const int len() const
    {
      return len_;
    }

  private:
    uint8_t *start_ = nullptr;
    int len_ = 0;
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

      if (spbuf_ != nullptr)
      {
        offset_ = spbuf_->data();
      }
    }

    ~SPBuffer()
    {
      spbuf_ = nullptr;
    }

    bool isNull()
    {
      return spbuf_ == nullptr;
    }

    uint8_t *offset(uint8_t *offset = nullptr)
    {
      if (offset != nullptr)
      {
        this->offset_ = offset;
      }

      return this->offset_;
    }

    uint8_t *data() const
    {
      if (spbuf_ == nullptr)
      {
        return nullptr;
      }

      return spbuf_->data();
    }

    const int len() const
    {
      if (spbuf_ == nullptr)
      {
        return 0;
      }

      return spbuf_->len();
    }

    SPBuffer dump(int len = -INT32_MIN)
    {
      if (spbuf_ == nullptr)
      {
        return nullVal();
      }

      if (len == -INT32_MIN)
      {
        return SPBuffer(spbuf_->len(), spbuf_->data());
      }

      if (len > spbuf_->len())
      {
        auto buf = std::make_shared<Buffer>(len);
        memcpy(buf->data(), spbuf_->data(), spbuf_->len());
        memset(&buf->data()[spbuf_->len()], 0, len - spbuf_->len());
        return SPBuffer(buf->len(), buf->data());
      }

      return SPBuffer(len, spbuf_->data());
    }

    SPBuffer frontInsert(int len, uint8_t *data = nullptr)
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

    SPBuffer backInsert(int len, uint8_t *data = nullptr)
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
    uint8_t *offset_ = nullptr;
  };
}