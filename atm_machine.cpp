#include "atm_machine.h"

#include <iostream>

#include "atm_messages.h"
#include "dispatcher.h"

ATMMachine::ATMMachine(const messaging::Sender &bank,
                       const messaging::Sender &hardware)
    : bank_(bank), hardware_(hardware) {}

messaging::Sender ATMMachine::GetSender() { return incoming_; }

void ATMMachine::Run() {
  state_ = &ATMMachine::WaitingForCard;
  try {
    for (;;) {
      (this->*state_)();
    }
  } catch (const messaging::CloseQueue &) {
  }
}

void ATMMachine::Done() {
    GetSender().Send(messaging::CloseQueue());
}

void ATMMachine::WaitingForCard() {
  hardware_.Send(DisplayEnterCard());
  incoming_.Wait().Handle<CardInserted>(
      [this](const CardInserted &card_inserted) {
        pin_ = "";
        hardware_.Send(DisplayEnterPin());
        state_ = &ATMMachine::GettingPin;
      });
}

void ATMMachine::GettingPin() {
  incoming_.Wait()
      .Handle<DigitPressed>([this](const DigitPressed &message) {
        pin_ += message.digit_;
        if (pin_.size() == 6) {
          bank_.Send(VerifyPin(pin_, incoming_));
          state_ = &ATMMachine::VerifyingPin;
        }
      })
      .Handle<CancelPressed>([this](const CancelPressed &) {
        if (!pin_.empty()) {
          pin_.pop_back();
        }
      });
}

void ATMMachine::VerifyingPin() {
  incoming_.Wait()
      .Handle<PinCorrect>([this](const PinCorrect &message) {
        name_ = message.name_;
        state_ = &ATMMachine::WaitingForAction;
      })
      .Handle<PinIncorrect>([this](const PinIncorrect &) {
        state_ = &ATMMachine::DoneProcessing;
      });
}

void ATMMachine::WaitingForAction() {
  hardware_.Send(DisplayOptions(name_));
  incoming_.Wait()
      .Handle<BalancePressed>([this](const BalancePressed &) {
        bank_.Send(GetBalance(incoming_));
        state_ = &ATMMachine::ProcessBalance;
      })
      .Handle<CancelPressed>([this](const CancelPressed &) {
        state_ = &ATMMachine::DoneProcessing;
      })
      .Handle<WithdrawPressed>([this](const WithdrawPressed &message) {
        hardware_.Send(DisplayWithdrawOptions());
        state_ = &ATMMachine::ProcessWithdrawAmount;
      });
}

void ATMMachine::ProcessBalance() {
  incoming_.Wait()
      .Handle<Balance>([this](const Balance &message) {
        hardware_.Send(DisplayBalance(message.balance_));
        state_ = &ATMMachine::WaitingForAction;
      })
      .Handle<CancelPressed>([this](const CancelPressed &) {
        state_ = &ATMMachine::DoneProcessing;
      });
}

void ATMMachine::ProcessWithdrawAmount() {
  incoming_.Wait().Handle<DigitPressed>([this](const DigitPressed &message) {
    int amount = 0;
    switch (message.digit_) {
    case '1':
      amount = 50;
      break;
    case '2':
      amount = 100;
      break;
    case '3':
      amount = 200;
    case '4':
      amount = 500;
      break;
    default:
      state_ = &ATMMachine::WaitingForAction;
      return;
    }
    bank_.Send(WithdrawAmount(amount, incoming_));
    state_ = &ATMMachine::ProcessWithdraw;
  });
}

void ATMMachine::ProcessWithdraw() {
  incoming_.Wait()
      .Handle<WithdrawFailed>([this](const WithdrawFailed &) {
        hardware_.Send(DisplayWithdrawFailed());
        state_ = &ATMMachine::WaitingForAction;
      })
      .Handle<WithdrawSucceeded>([this](const WithdrawSucceeded &message) {
        hardware_.Send(DisplayWithdrawSucceeded(message.balance_));
        state_ = &ATMMachine::WaitingForAction;
      });
}

void ATMMachine::DoneProcessing() {
  hardware_.Send(EjectCard());
  state_ = &ATMMachine::WaitingForCard;
}