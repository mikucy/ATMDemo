#include "bank_machine.h"

#include "atm_messages.h"
#include "dispatcher.h"
#include "sender.h"

BankMachine::BankMachine(const std::string &name, const std::string &pin,
                         int balance)
    : name_(name), pin_(pin), balance_(balance) {}

void BankMachine::Run() {
  try {
    for (;;) {
      incoming_.Wait()
          .Handle<VerifyPin>([this](const VerifyPin &message) {
            if (message.pin_ == pin_) {
              message.sender_.Send(PinCorrect(name_));
            } else {
              message.sender_.Send(PinIncorrect());
            }
          })
          .Handle<GetBalance>([this](const GetBalance &message) {
            message.sender_.Send(Balance(balance_));
          })
          .Handle<WithdrawAmount>([this](const WithdrawAmount &message) {
            if (message.amount_ > balance_) {
              message.sender_.Send(WithdrawFailed());
            } else {
              balance_ -= message.amount_;
              message.sender_.Send(WithdrawSucceeded(balance_));
            }
          });
    }
  } catch (const messaging::CloseQueue &) {
  }
}

void BankMachine::Done() { GetSender().Send(messaging::CloseQueue()); }

messaging::Sender BankMachine::GetSender() { return incoming_; }