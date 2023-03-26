// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <device_ob1_win.hpp>
#include <message_distributor.hpp>
#include <sentry_worker.hpp>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {
  Messages::MessageDistributor messageDistributor;

  messageDistributor.addParticipant(
      shared_ptr<MessageInterface>(new Devices::DeviceIsx3()));
  messageDistributor.addParticipant(
      shared_ptr<MessageInterface>(new Devices::DeviceOb1Win()));

  shared_ptr<Workers::SentryWorker> sentryWorker(new Workers::SentryWorker());
  messageDistributor.addParticipant(sentryWorker);

  sentryWorker->start();

  while (true) {
    messageDistributor.deliverMessages();
  }

  return 0;
}