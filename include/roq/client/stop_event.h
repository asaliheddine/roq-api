/* Copyright (c) 2017-2020, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include "roq/compat.h"

namespace roq {
namespace client {

struct ROQ_PUBLIC StopEvent final {
};

}  // namespace client
}  // namespace roq

template <>
struct fmt::formatter<roq::client::StopEvent> {
  template <typename Context>
  constexpr auto parse(Context& context) {
    return context.begin();
  }
  template <typename Context>
  auto format(
      const roq::client::StopEvent& value,
      Context& context) {
    return format_to(
        context.out(),
        R"({{)"
        R"(}})");
  }
};
