// 3rd party includes
#include <Elveflow64.h>
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <device_ob1_win.hpp>
#include <message_distributor.hpp>
#include <sentry_init_payload.hpp>
#include <sentry_worker.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Messages;
using namespace Devices;
using namespace Workers;

#define DEFAULT_MESSAGE_DISTRIBUTOR_LOOP_INTERVAL 50

int main(int argc, char *argv[]) {
  LOG(INFO) << "Starting up sentry.";

  // Create the distributor.
  MessageDistributor messageDistributor(
      DEFAULT_MESSAGE_DISTRIBUTOR_LOOP_INTERVAL);

  // Create Devices and add them to the distributor immediatelly.
  messageDistributor.addParticipant(
      shared_ptr<MessageInterface>(new DeviceIsx3()));
  messageDistributor.addParticipant(
      shared_ptr<MessageInterface>(new DeviceOb1Win()));

  // Create the worker and add it to the distributor.
  shared_ptr<MessageInterface> sentryWorker(new SentryWorker());
  messageDistributor.addParticipant(sentryWorker);
  // Queue up messages for the worker, so that it starts interacting with the
  // devices immediatelly.
  messageDistributor.takeMessage(
      shared_ptr<DeviceMessage>(new InitDeviceMessage(
          shared_ptr<MessageInterface>(), sentryWorker,
          new SentryInitPayload(
              new Isx3InitPayload("127.0.0.1", 8888),
              new Isx3IsConfPayload(
                  10.0, 1000.0, 10, 0,
                  map<ChannelFunction, int>({{CHAN_FUNC_CP, 10},
                                             {CHAN_FUNC_RP, 10},
                                             {CHAN_FUNC_WS, 11},
                                             {CHAN_FUNC_WP, 11}}),
                  IsScale::LINEAR_SCALE, MEAS_CONFIG_RANGE_10MA,
                  MEAS_CONFIG_CHANNEL_EXT_PORT_2, MEAS_CONFIG_4_POINT, 1.0,
                  1.0),
              new Ob1InitPayload("01F8E63F",
                                 make_tuple(Z_regulator_type__0_2000_mbar,
                                            Z_regulator_type__0_2000_mbar,
                                            Z_regulator_type_none,
                                            Z_regulator_type_none)),

              true, chrono::seconds(5), chrono::seconds(10)))));
  messageDistributor.takeMessage(
      shared_ptr<DeviceMessage>(new ConfigDeviceMessage(
          shared_ptr<MessageInterface>(), sentryWorker, nullptr)));

  // Start the message distribution.
  messageDistributor.run();

  return 0;
}