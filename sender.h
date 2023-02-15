#ifndef ATM_SENDER_H_
#define ATM_SENDER_H_

#include "message_queue.h"

namespace messaging {
class Sender {
public:
  explicit Sender(MessageQueue *queue) : queue_(queue) {}
  template <typename Msg> void Send(const Msg &msg) {
    if (!queue_) {
      return;
    }
    queue_->Push(msg);
  }

private:
  MessageQueue *queue_ = nullptr;
};
} // namespace messaging
#endif // ATM_SENDER_H_