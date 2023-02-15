#ifndef HARDWARE_MACHINE_H_
#define HARDWARE_MACHINE_H_

#include "receiver.h"

class HardwareMachine {
public:
  void Run();
  void Done();

  messaging::Sender GetSender() { return incoming_; }

private:
  messaging::Receiver incoming_;
};
#endif // HARDWARE_MACHINE_H_