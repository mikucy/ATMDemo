#include "atm_machine.h"
#include "atm_messages.h"
#include "bank_machine.h"
#include "hardware_machine.h"
#include "sender.h"

#include <iostream>
#include <thread>

std::string GetAccountName() {
  std::string name;
  std::cout << "Please input your account name: " << std::endl;
  std::cin >> name;
  while (name.empty()) {
    std::cerr << "Name can't be empty! Please input again: " << std::endl;
    std::cin >> name;
  }
  return name;
}

bool CheckPinFormat(const std::string &pin) {
  if (pin.size() != 6) {
    return false;
  }
  for (auto c : pin) {
    if (c < '0' || c > '9') {
      return false;
    }
  }
  return true;
}

std::string GetPin() {
  std::string pin;
  std::cout << "Please input the pin: " << std::endl;
  std::cin >> pin;
  while (!CheckPinFormat(pin)) {
    std::cerr << "Pin should be 6 number digits! Please input again: "
              << std::endl;
    std::cin >> pin;
  }
  return pin;
}

int GetBalance() {
  std::cout << "Please input the balance: " << std::endl;
  int balance;
  std::cin >> balance;
  while (balance < 0) {
    std::cerr << "Balance can't be negative! Please input again: " << std::endl;
    std::cin >> balance;
  }
  return balance;
}

int main() {
  std::cout << "Welcome to the ATM simulation!" << std::endl;

  // Prompt for the account name.
  const auto name = GetAccountName();

  // Prompt for the pin.
  const auto pin = GetPin();

  // Prompt for the balance.
  const auto balance = GetBalance();

  // Start simulation.
  std::cout << "Your bank account has been established. Let's start! You can "
               "press \"q\" to quit the program"
            << std::endl;

  BankMachine bank_machine(name, pin, balance);
  HardwareMachine hardware_machine;
  ATMMachine atm_machine(bank_machine.GetSender(),
                         hardware_machine.GetSender());
  messaging::Sender sender(atm_machine.GetSender());
  std::thread atm_thread(&ATMMachine::Run, &atm_machine);
  std::thread bank_thread(&BankMachine::Run, &bank_machine);
  std::thread hardware_thread(&HardwareMachine::Run, &hardware_machine);

  bool quit = false;
  while (!quit) {
    char c;
    std::cin >> c;
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      sender.Send(DigitPressed(c));
      break;
    case 'b':
      sender.Send(BalancePressed());
      break;
    case 'c':
      sender.Send(CancelPressed());
      break;
    case 'i':
      sender.Send(CardInserted());
      break;
    case 'q':
      quit = true;
      break;
    case 'w':
      sender.Send(WithdrawPressed());
    default:
      break;
    }
  }

  atm_machine.Done();
  hardware_machine.Done();
  bank_machine.Done();
  atm_thread.join();
  hardware_thread.join();
  bank_thread.join();
}