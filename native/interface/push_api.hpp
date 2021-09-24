
#include <memory>

#include "push_event.hpp"
#include "enctrl.hpp"

namespace xlab::PushAPI
{
    void setLogPath(const char *path);

    void setPushEventCallBack(std::shared_ptr<IPushEvent> event);

    [[maybe_unused]] void setEncodecCtrlCallBack(std::shared_ptr<IEnctrl> enctrl);

    ///pktType: 0 is non-keyframe-video, 1 is keyframe-video, 2 is audio
    void push(const char *url,
              const uint8_t *head,
              const int headLen,
              const uint8_t *body,
              const int bodyLen,
              const int64_t dtsUs,
              const int pktType,
              const int rotate);

} // namespace xlab
