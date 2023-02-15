#ifndef ATM_RECEIVER_H_
#define ATM_RECEIVER_H_

#include "dispatcher.h"
#include "sender.h"

namespace messaging {
class Receiver {
public:
  Dispatcher Wait() { return Dispatcher(&queue_); }
  operator Sender() { return Sender(&queue_); }

private:
  MessageQueue queue_;
};
} // namespace messaging
#endif // ATM_RECEIVER_H_