// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <init_device_message.hpp>
#include <message_distributor.hpp>
#include <network_worker.hpp>
#include <network_worker_init_payload.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Workers;
using namespace Messages;

TEST_CASE("Test the network workers") {
  MessageDistributor distributorServer(100);
  MessageDistributor distributorClient(100);
  shared_ptr<MessageInterface> server(new NetworkWorker());
  shared_ptr<MessageInterface> client(new NetworkWorker());
  shared_ptr<NetworkWorker> serverWorker =
      dynamic_pointer_cast<NetworkWorker>(server);
  shared_ptr<NetworkWorker> clientWorker =
      dynamic_pointer_cast<NetworkWorker>(client);

  distributorServer.addParticipant(server);
  distributorClient.addParticipant(client);

  shared_ptr<InitDeviceMessage> serverInitMessage(new InitDeviceMessage(
      UserId(), server->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER,
          "127.0.0.1", 54321)));
  shared_ptr<InitDeviceMessage> clientInitMessage(new InitDeviceMessage(
      UserId(), client->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT,
          "127.0.0.1", 54321)));

  REQUIRE(server->write(serverInitMessage));
  REQUIRE(client->write(clientInitMessage));

  shared_ptr<ConfigDeviceMessage> serverConfigMessage(
      new ConfigDeviceMessage(UserId(), server->getUserId(), nullptr));
  shared_ptr<ConfigDeviceMessage> clientConfigMessage(
      new ConfigDeviceMessage(UserId(), client->getUserId(), nullptr));

  REQUIRE(server->write(serverConfigMessage));
  REQUIRE(client->write(clientConfigMessage));

  shared_ptr<WriteDeviceMessage> serverStartMessage(new WriteDeviceMessage(
      UserId(), server->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));
  shared_ptr<WriteDeviceMessage> clientStartMessage(new WriteDeviceMessage(
      UserId(), client->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));

  REQUIRE(server->write(serverStartMessage));
  this_thread::sleep_for(chrono::milliseconds(1000));
  REQUIRE(client->write(clientStartMessage));

  this_thread::sleep_for(chrono::milliseconds(1000));

  // Workers should now both be in operating state.
  REQUIRE(serverWorker->getState() == DeviceStatus::OPERATING);
  REQUIRE(clientWorker->getState() == DeviceStatus::OPERATING);
  // Workers should now have each others user id as proxy id.
  REQUIRE(serverWorker->getProxyUserIds() ==
          list<UserId>{clientWorker->getUserId()});
  REQUIRE(clientWorker->getProxyUserIds() ==
          list<UserId>{serverWorker->getUserId()});
}
