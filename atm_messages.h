#ifndef ATM_MESSAGE_H_
#define ATM_MESSAGE_H_

#include "sender.h"
#include <string>

struct CardInserted {};

struct DigitPressed {
  explicit DigitPressed(char digit) : digit_(digit) {}
  char digit_;
};

struct CancelPressed {};

struct QuitPressed {};

struct BalancePressed {};

struct WithdrawPressed {};

struct Balance {
  explicit Balance(int balance) : balance_(balance) {}
  int balance_ = 0;
};

// Hardware
struct DisplayEnterCard {};

struct DisplayEnterPin {};

struct DisplayOptions {
  explicit DisplayOptions(const std::string &name) : name_(name) {}
  std::string name_;
};

struct DisplayBalance {
  explicit DisplayBalance(int balance) : balance_(balance) {}
  int balance_ = 0;
};

struct DisplayWithdrawOptions {};

struct DisplayWithdrawFailed {};

struct DisplayWithdrawSucceeded {
  explicit DisplayWithdrawSucceeded(int balance) : balance_(balance) {}
  int balance_ = 0;
};

struct EjectCard {};

// Bank
struct VerifyPin {
  VerifyPin(const std::string &pin, const messaging::Sender &sender)
      : pin_(pin), sender_(sender) {}
  std::string pin_;
  mutable messaging::Sender sender_;
};

struct PinCorrect {
  explicit PinCorrect(const std::string &name) : name_(name) {}
  std::string name_;
};

struct PinIncorrect {};

struct GetBalance {
  explicit GetBalance(const messaging::Sender &sender) : sender_(sender) {}
  mutable messaging::Sender sender_;
};

struct WithdrawAmount {
  WithdrawAmount(int amount, messaging::Sender sender)
      : amount_(amount), sender_(sender) {}
  int amount_ = 0;
  mutable messaging::Sender sender_;
};

struct WithdrawFailed {};

struct WithdrawSucceeded {
  explicit WithdrawSucceeded(int balance) : balance_(balance) {}
  int balance_ = 0;
};
#endif // ATM_MESSAGE_H_