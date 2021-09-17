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
      static SPBuffer bf = SPBuffer(0);
      return bf;
    }

  public:
    explicit SPBuffer(int len, uint8_t *data = nullptr)
    {
      spbuf = std::make_shared<Buffer>(len, data);
      if (spbuf == nullptr || spbuf->isNull())
      {
        spbuf == nullptr;
      }
    }

    ~SPBuffer()
    {
      spbuf = nullptr;
    }

    bool isNull()
    {
      spbuf == nullptr;
    }

    uint8_t *data() const
    {
      if (spbuf == nullptr)
      {
        return nullptr;
      }

      return spbuf->data();
    }

    const int len() const
    {
      if (spbuf == nullptr)
      {
        return 0;
      }

      return spbuf->len();
    }

    SPBuffer dump(int len = -INT32_MIN)
    {
      if (spbuf == nullptr)
      {
        return nullVal();
      }

      if (len == -INT32_MIN)
      {
        return SPBuffer(spbuf->len(), spbuf->data());
      }

      if (len > spbuf->len())
      {
        auto buf = std::make_shared<Buffer>(len);
        memcpy(buf->data(), spbuf->data(), spbuf->len());
        memset(&buf->data()[spbuf->len()], 0, len - spbuf->len());
        return SPBuffer(buf->len(), buf->data());
      }

      return SPBuffer(len, spbuf->data());
    }

    SPBuffer frontInsert(int len, uint8_t *data = nullptr)
    {
      if (spbuf == nullptr || len <= 0 || data == nullptr)
      {
        return nullVal();
      }

      auto buf = std::make_shared<Buffer>(spbuf->len() + len);
      memcpy(buf->data(), spbuf->data(), spbuf->len());
      memcpy(&buf->data()[spbuf->len()], data, len);
      return SPBuffer(buf->len(), buf->data());
    }

    SPBuffer backInsert(int len, uint8_t *data = nullptr)
    {
      if (spbuf == nullptr || len <= 0 || data == nullptr)
      {
        return nullVal();
      }

      auto buf = std::make_shared<Buffer>(len + spbuf->len());
      memcpy(buf->data(), data, len);
      memcpy(&buf->data()[len], spbuf->data(), spbuf->len());
      return SPBuffer(buf->len(), buf->data());
    }

  private:
    std::shared_ptr<Buffer> spbuf = nullptr;
  };
}