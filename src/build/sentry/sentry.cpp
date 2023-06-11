// 3rd party includes
#include <Elveflow64_shim.h>
#include <argparse.hpp>
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <device_isx3.hpp>
#include <device_ob1_win.hpp>
#include <isx3_payload_decoder.hpp>
#include <message_distributor.hpp>
#include <network_worker.hpp>
#include <ob1_payload_decoder.hpp>
#include <sentry_init_payload.hpp>
#include <sentry_payload_decoder.hpp>
#include <sentry_worker.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Messages;
using namespace Devices;
using namespace Workers;

#define DEFAULT_MESSAGE_DISTRIBUTOR_LOOP_INTERVAL 50

int main(int argc, char *argv[]) {
  LOG(INFO) << "Starting up sentry.";
  // Set up the command line parser.
  argparse::ArgumentParser program("SCIMon Sentry");
  program.add_argument("--port")
      .default_value(NetworkWorkerDefaultPort)
      .help("The port to which the network worker listens for connections.");
  program.add_argument("--interval")
      .default_value(DEFAULT_MESSAGE_DISTRIBUTOR_LOOP_INTERVAL)
      .help("The message distributor interval.");

  // Try to parse the command line.
  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    LOG(ERROR) << "Could not parse the command line. Aborting.";
    return 1;
  }

  // Create the distributor.
  MessageDistributor messageDistributor(program.get<int>("--interval"));
  // Initialize the message factory.
  MessageFactory::createInstace(
      {std::shared_ptr<PayloadDecoder>(new Ob1PayloadDecoder()),
       std::shared_ptr<PayloadDecoder>(new Isx3PayloadDecoder()),
       std::shared_ptr<PayloadDecoder>(new SentryPayloadDecoder())});

  // Create Devices and add them to the distributor immediatelly.
  messageDistributor.addParticipant(
      std::shared_ptr<MessageInterface>(new DeviceIsx3()));
  messageDistributor.addParticipant(
      std::shared_ptr<MessageInterface>(new DeviceOb1Win()));
  std::shared_ptr<MessageInterface> networkWorker(new NetworkWorker());
  messageDistributor.addParticipant(networkWorker);

  // Create the master worker and add it to the distributor.
  std::shared_ptr<MessageInterface> sentryWorker(new SentryWorker());
  messageDistributor.addParticipant(sentryWorker);

  // Immediatelly queue up messages for the network worker, so that it starts
  // listening.
  messageDistributor.takeMessage(
      std::shared_ptr<DeviceMessage>(new InitDeviceMessage(
          UserId(), networkWorker->getUserId(),
          new NetworkWorkerInitPayload(
              NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER, "",
              program.get<int>("--port")))));
  messageDistributor.takeMessage(std::shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(UserId(), networkWorker->getUserId(),
                             WriteDeviceTopic::WRITE_TOPIC_RUN)));

  // Start the message distribution.
  messageDistributor.run();

  return 0;
}