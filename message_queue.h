#ifndef ATM_MESSAGE_QUEUE_H_
#define ATM_MESSAGE_QUEUE_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace messaging {
struct MessageBase {
  virtual ~MessageBase() {}
};

template <typename Msg> struct WrapperMessage : public MessageBase {
public:
  explicit WrapperMessage(const Msg &content) : content_(content) {}
  Msg content_;
};

class MessageQueue {
public:
  template <typename Msg> void Push(const Msg &msg) {
    std::lock_guard<std::mutex> lg(m_);
    queue_.push(std::make_shared<WrapperMessage<Msg>>(msg));
    cv_.notify_all();
  }

  std::shared_ptr<MessageBase> WaitAndPop() {
    std::unique_lock<std::mutex> l(m_);
    cv_.wait(l, [&] { return !queue_.empty(); });
    auto f = queue_.front();
    queue_.pop();
    return f;
  }

private:
  std::mutex m_;
  std::condition_variable cv_;
  std::queue<std::shared_ptr<MessageBase>> queue_;
};
} // namespace messaging
#endif // ATM_MESSAGE_QUEUE_H_