#ifndef BANK_MACHINE_H_
#define BANK_MACHINE_H_

#include "receiver.h"

class BankMachine {
public:
  BankMachine(const std::string &name, const std::string &pin, int balance);
  void Run();
  void Done();

  messaging::Sender GetSender();

private:
  messaging::Receiver incoming_;
  std::string name_;
  std::string pin_;
  int balance_ = 0;
};
#endif // BANK_MACHINE_H_