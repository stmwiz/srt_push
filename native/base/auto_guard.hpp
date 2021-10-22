//
//  auto_guard.hpp
//  iBoxSignatureImage
//
//  Created by TBD on 2021/10/18.
//  Copyright © 2021 TBD. All rights reserved.
//

#pragma once

#include <functional>
#include <memory>
#include <type_traits>

namespace xlab {

template <typename T, typename InitRet = void, typename DeinitRet = void>
struct auto_guard final {
  using InitFunction = std::function<InitRet(T &t)>;
  using DeinitFunction = std::function<DeinitRet(T &t)>;

private:
  T &t;

  InitRet *init_ret{nullptr};
  InitFunction init;

  DeinitRet *deinit_ret{nullptr};
  DeinitFunction deinit;

public:
  explicit auto_guard(T &t, InitRet *init_ret, InitFunction init,
                      DeinitRet *deinit_ret, DeinitFunction deinit)
      : t(t), init_ret(init_ret), init(std::move(init)), deinit_ret(deinit_ret),
        deinit(std::move(deinit)) {
    _call_init();
  }

  template <std::enable_if_t<std::is_same<void, InitRet>::value> * = nullptr>
  explicit auto_guard(T &t, InitFunction init, DeinitRet *deinit_ret,
                      DeinitFunction deinit)
      : t(t), init_ret(nullptr), init(std::move(init)), deinit_ret(deinit_ret),
        deinit(std::move(deinit)) {
    _call_init();
  }

  template <std::enable_if_t<std::is_same<void, DeinitRet>::value> * = nullptr>
  explicit auto_guard(T &t, InitRet *init_ret, InitFunction init,
                      DeinitFunction deinit)
      : t(t), init_ret(init_ret), init(std::move(init)), deinit_ret(nullptr),
        deinit(std::move(deinit)) {
    _call_init();
  }

  template <std::enable_if_t<std::is_same<void, InitRet>::value &&
                             std::is_same<void, DeinitRet>::value> * = nullptr>
  explicit auto_guard(T &t, InitFunction init, DeinitFunction deinit)
      : t(t), init_ret(nullptr), init(std::move(init)), deinit_ret(nullptr),
        deinit(std::move(deinit)) {
    _call_init();
  }

  ~auto_guard() { _call_deinit(); }

private:
  void _call_init() {
    if (!init) {
      return;
    }

    if constexpr (std::is_same<void, InitRet>::value) {
      init(t);
    } else {
      if (init_ret) {
        *(init_ret) = this->init(t);
      } else {
        init(t);
      }
    }
  }

  void _call_deinit() {
    if (!deinit) {
      return;
    }

    if constexpr (std::is_same<void, DeinitRet>::value) {
      deinit(t);
    } else {
      if (deinit_ret) {
        *(deinit_ret) = deinit(t);
      } else {
        deinit(t);
      }
    }
  }
}; // struct auto_guard<T>

struct auto_guard_void final {
  using InitFunction = std::function<void()>;
  using DeinitFunction = std::function<void()>;

private:
  InitFunction init;
  DeinitFunction deinit;

public:
  explicit auto_guard_void(InitFunction init, DeinitFunction deinit)
      : init(std::move(init)), deinit(std::move(deinit)) {
    if (init != nullptr) {
      init();
    }
  }

  ~auto_guard_void() {
    if (deinit != nullptr) {
      deinit();
    }
  }
}; // struct auto_guard_void

} // namespace xlab
