#include "push_api.hpp"

namespace xlab::PushAPI
{
    void setLogPath(const char *path)
    {
    }

    void setPushEventCallBack(std::shared_ptr<IPushEvent> event)
    {
    }

    [[maybe_unused]] void setEncodecCtrlCallBack(std::shared_ptr<IEnctrl> enctrl)
    {
    }

    void push(const char *url,
              const uint8_t *head,
              const int headLen,
              const uint8_t *body,
              const int bodyLen,
              const int64_t dts_us,
              const int pktType,
              const int rotate)
    {
    }

} // namespace xlab::PushAPI
