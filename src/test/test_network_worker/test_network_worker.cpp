// Standard includes
#include <thread>

// 3rd party includes
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <easylogging++.h>

// Project includes
#include <device_isx3.hpp>
#include <device_ob1_win.hpp>
#include <init_device_message.hpp>
#include <isx3_payload_decoder.hpp>
#include <message_distributor.hpp>
#include <message_factory.hpp>
#include <network_worker.hpp>
#include <network_worker_init_payload.hpp>
#include <ob1_payload_decoder.hpp>
#include <test_device.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace Workers;
using namespace Messages;
using namespace Devices;

const int DefaultPort = 54321;
const std::string DefaultIp = "127.0.0.1";

TEST_CASE("Test the network worker handshake") {
  MessageFactory::createInstace(std::list<std::shared_ptr<PayloadDecoder>>{
      std::shared_ptr<PayloadDecoder>(new Isx3PayloadDecoder()),
      std::shared_ptr<PayloadDecoder>(new Ob1PayloadDecoder())});
  MessageDistributor distributorServer(100);
  MessageDistributor distributorClient(100);
  std::shared_ptr<MessageInterface> server(new NetworkWorker());
  std::shared_ptr<MessageInterface> client(new NetworkWorker());
  std::shared_ptr<NetworkWorker> serverWorker =
      dynamic_pointer_cast<NetworkWorker>(server);
  std::shared_ptr<NetworkWorker> clientWorker =
      dynamic_pointer_cast<NetworkWorker>(client);
  distributorServer.addParticipant(server);
  distributorClient.addParticipant(client);

  std::shared_ptr<InitDeviceMessage> serverInitMessage(new InitDeviceMessage(
      UserId(), server->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER, DefaultIp,
          DefaultPort)));
  std::shared_ptr<InitDeviceMessage> clientInitMessage(new InitDeviceMessage(
      UserId(), client->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT, DefaultIp,
          DefaultPort)));

  REQUIRE(server->write(serverInitMessage));
  REQUIRE(client->write(clientInitMessage));

  std::shared_ptr<ConfigDeviceMessage> serverConfigMessage(
      new ConfigDeviceMessage(UserId(), server->getUserId(), nullptr));
  std::shared_ptr<ConfigDeviceMessage> clientConfigMessage(
      new ConfigDeviceMessage(UserId(), client->getUserId(), nullptr));

  REQUIRE(server->write(serverConfigMessage));
  REQUIRE(client->write(clientConfigMessage));

  std::shared_ptr<WriteDeviceMessage> serverStartMessage(new WriteDeviceMessage(
      UserId(), server->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));
  std::shared_ptr<WriteDeviceMessage> clientStartMessage(new WriteDeviceMessage(
      UserId(), client->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));

  REQUIRE(server->write(serverStartMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  REQUIRE(client->write(clientStartMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Workers should now both be in operating state.
  REQUIRE(serverWorker->getState() == DeviceStatus::OPERATING);
  REQUIRE(clientWorker->getState() == DeviceStatus::OPERATING);
  // Workers should now have each others user id as proxy id.
  REQUIRE(serverWorker->getProxyUserIds() ==
          std::list<UserId>{clientWorker->getUserId()});
  REQUIRE(clientWorker->getProxyUserIds() ==
          std::list<UserId>{serverWorker->getUserId()});

  // Shut down the connection.
  std::shared_ptr<WriteDeviceMessage> serverStopMessage(new WriteDeviceMessage(
      UserId(), server->getUserId(), WriteDeviceTopic::WRITE_TOPIC_STOP));
  std::shared_ptr<WriteDeviceMessage> clientStopMessage(new WriteDeviceMessage(
      UserId(), client->getUserId(), WriteDeviceTopic::WRITE_TOPIC_STOP));

  REQUIRE(server->write(serverStopMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  REQUIRE(client->write(clientStopMessage));

  REQUIRE(serverWorker->getState() == DeviceStatus::IDLE);
  REQUIRE(clientWorker->getState() == DeviceStatus::IDLE);
}

TEST_CASE("Test the network worker reconnect") {
  MessageFactory::createInstace(std::list<std::shared_ptr<PayloadDecoder>>{
      std::shared_ptr<PayloadDecoder>(new Isx3PayloadDecoder()),
      std::shared_ptr<PayloadDecoder>(new Ob1PayloadDecoder())});
  MessageDistributor distributorServer(100);
  MessageDistributor distributorClient(100);
  std::shared_ptr<MessageInterface> server(new NetworkWorker());
  std::shared_ptr<MessageInterface> client(new NetworkWorker());
  std::shared_ptr<NetworkWorker> serverWorker =
      dynamic_pointer_cast<NetworkWorker>(server);
  std::shared_ptr<NetworkWorker> clientWorker =
      dynamic_pointer_cast<NetworkWorker>(client);
  distributorServer.addParticipant(server);
  distributorClient.addParticipant(client);

  std::shared_ptr<InitDeviceMessage> serverInitMessage(new InitDeviceMessage(
      UserId(), server->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER, DefaultIp,
          DefaultPort)));
  std::shared_ptr<InitDeviceMessage> clientInitMessage(new InitDeviceMessage(
      UserId(), client->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT, DefaultIp,
          DefaultPort)));

  REQUIRE(server->write(serverInitMessage));
  REQUIRE(client->write(clientInitMessage));

  std::shared_ptr<ConfigDeviceMessage> serverConfigMessage(
      new ConfigDeviceMessage(UserId(), server->getUserId(), nullptr));
  std::shared_ptr<ConfigDeviceMessage> clientConfigMessage(
      new ConfigDeviceMessage(UserId(), client->getUserId(), nullptr));

  REQUIRE(server->write(serverConfigMessage));
  REQUIRE(client->write(clientConfigMessage));

  std::shared_ptr<WriteDeviceMessage> serverStartMessage(new WriteDeviceMessage(
      UserId(), server->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));
  std::shared_ptr<WriteDeviceMessage> clientStartMessage(new WriteDeviceMessage(
      UserId(), client->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));

  REQUIRE(server->write(serverStartMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  REQUIRE(client->write(clientStartMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Workers should now both be in operating state.
  REQUIRE(serverWorker->getState() == DeviceStatus::OPERATING);
  REQUIRE(clientWorker->getState() == DeviceStatus::OPERATING);
  // Workers should now have each others user id as proxy id.
  REQUIRE(serverWorker->getProxyUserIds() ==
          std::list<UserId>{clientWorker->getUserId()});
  REQUIRE(clientWorker->getProxyUserIds() ==
          std::list<UserId>{serverWorker->getUserId()});

  // Let the client shut down the connection.
  std::shared_ptr<WriteDeviceMessage> clientStopMessage(new WriteDeviceMessage(
      UserId(), client->getUserId(), WriteDeviceTopic::WRITE_TOPIC_STOP));
  REQUIRE(client->write(clientStopMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  REQUIRE(serverWorker->getState() == DeviceStatus::BUSY);
  REQUIRE(clientWorker->getState() == DeviceStatus::IDLE);

  // Client reconnects.
  std::shared_ptr<WriteDeviceMessage> clientReconnectMessage(
      new WriteDeviceMessage(UserId(), client->getUserId(),
                             WriteDeviceTopic::WRITE_TOPIC_RUN));
  REQUIRE(client->write(clientReconnectMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  REQUIRE(serverWorker->getState() == DeviceStatus::OPERATING);
  REQUIRE(clientWorker->getState() == DeviceStatus::OPERATING);

  // Shut down both network workers.
  std::shared_ptr<WriteDeviceMessage> serverStopMessage(new WriteDeviceMessage(
      UserId(), server->getUserId(), WriteDeviceTopic::WRITE_TOPIC_STOP));
  REQUIRE(server->write(serverStopMessage));
  REQUIRE(client->write(clientStopMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  REQUIRE(serverWorker->getState() == DeviceStatus::IDLE);
  REQUIRE(clientWorker->getState() == DeviceStatus::IDLE);
}

TEST_CASE("Test communication between the end points") {
  MessageFactory::createInstace(std::list<std::shared_ptr<PayloadDecoder>>{
      std::shared_ptr<PayloadDecoder>(new Isx3PayloadDecoder()),
      std::shared_ptr<PayloadDecoder>(new Ob1PayloadDecoder())});
  MessageDistributor distributorServer(100);
  MessageDistributor distributorClient(100);
  std::shared_ptr<MessageInterface> server(new NetworkWorker());
  std::shared_ptr<MessageInterface> client(new NetworkWorker());
  std::shared_ptr<NetworkWorker> serverWorker =
      dynamic_pointer_cast<NetworkWorker>(server);
  std::shared_ptr<NetworkWorker> clientWorker =
      dynamic_pointer_cast<NetworkWorker>(client);
  distributorServer.addParticipant(server);
  distributorClient.addParticipant(client);
  std::shared_ptr<TestDevice> serverDevice(new TestDevice());
  std::shared_ptr<TestDevice> clientDevice(new TestDevice());
  distributorServer.addParticipant(serverDevice);
  distributorClient.addParticipant(clientDevice);

  std::shared_ptr<InitDeviceMessage> serverInitMessage(new InitDeviceMessage(
      UserId(), server->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER, DefaultIp,
          DefaultPort)));
  std::shared_ptr<InitDeviceMessage> clientInitMessage(new InitDeviceMessage(
      UserId(), client->getUserId(),
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT, DefaultIp,
          DefaultPort)));

  REQUIRE(server->write(serverInitMessage));
  REQUIRE(client->write(clientInitMessage));

  std::shared_ptr<ConfigDeviceMessage> serverConfigMessage(
      new ConfigDeviceMessage(UserId(), server->getUserId(), nullptr));
  std::shared_ptr<ConfigDeviceMessage> clientConfigMessage(
      new ConfigDeviceMessage(UserId(), client->getUserId(), nullptr));

  REQUIRE(server->write(serverConfigMessage));
  REQUIRE(client->write(clientConfigMessage));

  std::shared_ptr<WriteDeviceMessage> serverStartMessage(new WriteDeviceMessage(
      UserId(), server->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));
  std::shared_ptr<WriteDeviceMessage> clientStartMessage(new WriteDeviceMessage(
      UserId(), client->getUserId(), WriteDeviceTopic::WRITE_TOPIC_RUN));

  REQUIRE(server->write(serverStartMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  REQUIRE(client->write(clientStartMessage));
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Workers should now both be in operating state.
  REQUIRE(serverWorker->getState() == DeviceStatus::OPERATING);
  REQUIRE(clientWorker->getState() == DeviceStatus::OPERATING);
  // Workers should now have each others user id as proxy id.
  REQUIRE(
      serverWorker->getProxyUserIds() ==
      std::list<UserId>{clientWorker->getUserId(), clientDevice->getUserId()});
  REQUIRE(
      clientWorker->getProxyUserIds() ==
      std::list<UserId>{serverWorker->getUserId(), serverDevice->getUserId()});

  clientDevice->start();
  std::thread serverThread(&MessageDistributor::run, &distributorServer);
  std::thread clientThread(&MessageDistributor::run, &distributorClient);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  REQUIRE(clientDevice->returnReceivedVector() ==
          std::vector<unsigned char>{1, 2, 3});

  distributorServer.stop();
  distributorClient.stop();
  serverThread.join();
  clientThread.join();
}
