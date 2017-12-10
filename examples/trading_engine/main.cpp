/* Copyright (c) 2017, Hans Erik Thrane */

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <quinclas/client/event_dispatcher.h>
#include <quinclas/client/request_dispatcher.h>

#include "your/strategy.h"

DEFINE_string(local_address, "", "local address (unix domain socket)");

int main(int argc, char *argv[]) {
  int result = EXIT_FAILURE;
  try {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    gflags::ShutDownCommandLineFlags();

    if (FLAGS_local_address.empty())
      throw std::runtime_error("local_address is missing");

    LOG(INFO) << "*** START ***";

    // local (unix domain) socket address
    quinclas::io::net::UnixAddress address(FLAGS_local_address.c_str());
    // initialize libevent base
    quinclas::io::libevent::Base base;
    // create a socket and wrap it for use by libevent
    quinclas::io::net::Socket socket(PF_LOCAL, SOCK_STREAM, 0);
    socket.non_blocking(true);
    quinclas::io::libevent::BufferEvent buffer_event(base, std::move(socket));
    // create strategy (including request dispatcher, event dispatcher and timer)
    quinclas::client::RequestDispatcher request_dispatcher(buffer_event);
    examples::your::Strategy strategy(request_dispatcher);
    quinclas::client::EventDispatcher event_dispatcher(strategy, base, buffer_event);
    quinclas::io::libevent::TimerEvent timer(event_dispatcher, base);
    struct timeval timeout{ .tv_sec = 1, .tv_usec = 0 };
    timer.add(&timeout);
    // connect the socket
    buffer_event.connect(address);
    // start the libevent loop
    base.loop(EVLOOP_NO_EXIT_ON_EMPTY);

    result = EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    LOG(ERROR) << "Exception: " << e.what();
  }
  catch (...) {
    LOG(ERROR) << "Exception: <unknown>";
  }

  LOG(INFO) << "*** STOP *** (exit code: " << result << ")";

  return result;
}