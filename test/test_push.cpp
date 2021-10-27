#include <bits/types/FILE.h>

#include <thread>

#include "base/auto_guard.hpp"
#include "base/buffer.hpp"
#include "base/xlog.hpp"
#include "srtcore/srt_push.hpp"

using namespace xlab;

static void test_ts_srt_push(void *arg) {
    bool *quit = (bool *)arg;
    const char *tsUrl = "/home/x/Videos/gtx4k.ts";
    FILE *readTs = ::fopen(tsUrl, "rb");
    if (readTs == nullptr) {
        xloge("fopen {} fail !!!", tsUrl);
        return;
    }

    xlogi("fopen {} ok", tsUrl);

    auto_guard_void cleanUp(nullptr, [&readTs, &tsUrl] {
        if (readTs != nullptr) {
            ::fclose(readTs);
            readTs = nullptr;
            xlogi("fclose {}", tsUrl);
        }
    });

    const char *ip = "192.168.43.156";
    const int port = 34597;
    const char *streamid = "cast.uplive/live/camera0";
    auto srtpush = std::make_shared<SRTPush>(ip, port, streamid);
    if (!srtpush->success()) {
        xloge("srt push {}:{} fail !!!", ip, port);
        return;
    }

    auto readTsBuf = std::make_shared<base::Buffer>(1316);
    while (!*quit) {
        readTsBuf->bZero();
        auto retSize = ::fread(readTsBuf->start(), 1,readTsBuf->len(), readTs);
        if (retSize != readTsBuf->len()) {
            ::fseek(readTs, 0, SEEK_END);
            continue;
        }

        const auto ret = srtpush->write(readTsBuf);
        xlogi("srtpush write,ret({})", ret);
    }
}

int main() {
    xlog.setTextPath("sdk.log");
    xlogi("run main...");

    xlogi("run push thread...");
    bool quit = false;
    auto task = std::thread(test_ts_srt_push, &quit);
    while (getchar() != 'q') {
    }

    quit = true;
    if (task.joinable()) {
        task.join();
    }
    xlogi("over push thread!!!");

    xlogi("over main!!!");
    return 0;
}
