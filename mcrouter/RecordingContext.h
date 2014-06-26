/**
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <string>
#include <vector>

#include "folly/Optional.h"
#include "mcrouter/lib/McRequestWithContext.h"
#include "mcrouter/lib/Operation.h"
#include "mcrouter/lib/fibers/FiberPromise.h"

namespace facebook { namespace memcache {

class McReply;

namespace mcrouter {

class ProxyClientCommon;

/**
 * With this context, the requests are not actually sent out
 * over the network, we only record where the requests would be sent.
 */
class RecordingContext {
 public:

  struct RecordedData {
    std::vector<std::string> destinations;
  };

  RecordingContext();

  ~RecordingContext();

  void recordDestination(const ProxyClientCommon& destination);

  /**
   * Waits until all owners (i.e. requests) of ctx expire on other
   * fibers and returns the resulting vector of recorded destinations.
   */
  static
  std::unique_ptr<RecordedData> waitForRecorded(
    std::shared_ptr<RecordingContext>&& ctx);

 private:
  std::unique_ptr<RecordedData> recordedData_;
  folly::Optional<FiberPromise<std::unique_ptr<RecordedData>>> promise_;
};

typedef McRequestWithContext<RecordingContext> RecordingMcRequest;

} // mcrouter

template <typename Operation>
struct ReplyType<Operation, mcrouter::RecordingMcRequest> {
  typedef McReply type;
};

}}  // facebook::memcache