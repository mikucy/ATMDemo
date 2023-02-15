#include "hardware_machine.h"

#include <iostream>

#include "atm_messages.h"
#include "dispatcher.h"

void HardwareMachine::Run() {
  try {
    for (;;) {
      incoming_.Wait()
          .Handle<DisplayEnterCard>([](const DisplayEnterCard &) {
            std::cout << "Please press \"i\" to enter your card" << std::endl;
          })
          .Handle<DisplayEnterPin>([](const DisplayEnterPin &) {
            std::cout << "Please enter your pin: " << std::endl;
          })
          .Handle<DisplayOptions>([](const DisplayOptions &message) {
            std::cout << std::endl;
            std::cout << "Account name: " << message.name_ << std::endl;
            std::cout << "Options: " << std::endl;
            std::cout << "- b: Show balance" << std::endl;
            std::cout << "- w: Withdraw" << std::endl;
            std::cout << "- c: Eject the card" << std::endl;
            std::cout << std::endl;
          })
          .Handle<EjectCard>([](const EjectCard &) {
            std::cout << "Your card has been ejected" << std::endl;
            std::cout << "--------------------------" << std::endl;
          })
          .Handle<DisplayBalance>([](const DisplayBalance &message) {
            std::cout << "Account balance: " << message.balance_ << std::endl;
          })
          .Handle<DisplayWithdrawOptions>([](const DisplayWithdrawOptions &) {
            std::cout << "Please select the amount to withdraw:" << std::endl;
            std::cout << "- 1: 50" << std::endl;
            std::cout << "- 2: 100" << std::endl;
            std::cout << "- 3: 200" << std::endl;
            std::cout << "- 4: 500" << std::endl;
          })
          .Handle<DisplayWithdrawFailed>([](const DisplayWithdrawFailed &) {
            std::cout << "Account balance insufficient!" << std::endl;
          })
          .Handle<DisplayWithdrawSucceeded>(
              [](const DisplayWithdrawSucceeded &message) {
                std::cout << "Withdraw succeeded! Account balance: "
                          << message.balance_ << std::endl;
              });
    }
  } catch (const messaging::CloseQueue &) {
  }
}

void HardwareMachine::Done() { GetSender().Send(messaging::CloseQueue()); }