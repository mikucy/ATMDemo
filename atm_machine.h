#ifndef ATM_MACHINE_H_
#define ATM_MACHINE_H_

#include "receiver.h"
#include "sender.h"

class ATMMachine {
public:
  ATMMachine(const messaging::Sender &bank, const messaging::Sender &hardware);

  messaging::Sender GetSender();

  void Run();
  void Done();

private:
  void WaitingForCard();
  void GettingPin();
  void VerifyingPin();
  void WaitingForAction();
  void ProcessBalance();
  void ProcessWithdrawAmount();
  void ProcessWithdraw();
  void DoneProcessing();

private:
  messaging::Receiver incoming_;
  messaging::Sender bank_;
  messaging::Sender hardware_;
  void (ATMMachine::*state_)();

  std::string pin_;
  std::string name_;
};
#endif // ATM_MACHINE_H_