#ifndef ATM_DISPATCHER_H_
#define ATM_DISPATCHER_H_

#include "message_queue.h"

namespace messaging {
class MessageQueue;
struct CloseQueue {};

template <typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
public:
  TemplateDispatcher(MessageQueue *queue,
                     PreviousDispatcher *previous_dispatcher, Func &&f)
      : queue_(queue), previous_dispatcher_(previous_dispatcher),
        f_(std::forward<Func>(f)) {
    previous_dispatcher->chained_ = true;
  }
  TemplateDispatcher(TemplateDispatcher &&other)
      : queue_(other.queue_), chained_(other.chained_),
        previous_dispatcher_(other.previous_dispatcher_),
        f_(std::move(other.f_)) {
    other.chained_ = true;
  }
  TemplateDispatcher(const TemplateDispatcher &other) = delete;
  TemplateDispatcher &operator=(const TemplateDispatcher &other) = delete;
  template <typename Dispatcher, typename OtherMsg, typename OtherFunc>
  friend class TemplateDispatcher;

  ~TemplateDispatcher() noexcept(false) {
    if (chained_) {
      return;
    }
    WaitAndDispatch();
  }

  template <typename OtherMsg, typename OtherFunc>
  TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
  Handle(OtherFunc &&other_func) {
    return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
        queue_, this, std::forward<OtherFunc>(other_func));
  }

private:
  void WaitAndDispatch() {
    if (!queue_) {
      return;
    }
    for (;;) {
      auto message = queue_->WaitAndPop();
      if (Dispatch(message)) {
        break;
      }
    }
  }

  bool Dispatch(const std::shared_ptr<MessageBase> &msg) {
    if (auto *wrapper_msg = dynamic_cast<WrapperMessage<Msg> *>(msg.get())) {
      f_(wrapper_msg->content_);
      return true;
    } else {
      return previous_dispatcher_->Dispatch(msg);
    }
  }

private:
  MessageQueue *queue_ = nullptr;
  PreviousDispatcher *previous_dispatcher_ = nullptr;
  Func f_;
  bool chained_ = false;
};

class Dispatcher {
public:
  explicit Dispatcher(MessageQueue *queue) : queue_(queue) {}
  Dispatcher(Dispatcher &&other)
      : queue_(other.queue_), chained_(other.chained_) {
    other.chained_ = true;
  }
  Dispatcher(const Dispatcher &other) = delete;
  Dispatcher &operator=(const Dispatcher &other) = delete;
  template <typename Dispatcher, typename Msg, typename Func>
  friend class TemplateDispatcher;

  ~Dispatcher() noexcept(false) {
    if (chained_) {
      return;
    }
    WaitAndDispatch();
  }

  template <typename Msg, typename Func>
  TemplateDispatcher<Dispatcher, Msg, Func> Handle(Func &&f) {
    return TemplateDispatcher<Dispatcher, Msg, Func>(queue_, this,
                                                     std::forward<Func>(f));
  }

private:
  void WaitAndDispatch() {
    if (!queue_) {
      return;
    }
    for (;;) {
      auto message = queue_->WaitAndPop();
      Dispatch(message);
    }
  }

  bool Dispatch(const std::shared_ptr<MessageBase> &msg) {
    if (auto *wrapper_msg =
            dynamic_cast<WrapperMessage<CloseQueue> *>(msg.get())) {
      throw CloseQueue();
    }
    return false;
  }

private:
  MessageQueue *queue_ = nullptr;
  bool chained_ = false;
};
} // namespace messaging
#endif // ATM_DISPATCHER_H_