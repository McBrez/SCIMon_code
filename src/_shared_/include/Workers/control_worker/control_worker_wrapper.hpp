#ifndef CONTROL_WORKER_WRAPPER_HPP
#define CONTROL_WORKER_WRAPPER_HPP

// Standard includes
#include <memory>
#include <thread>

// Qt includes
#include <qobject>
#include <qthread>

// Project includes
#include <control_worker.hpp>
#include <isx3_payload_decoder.hpp>
#include <message_distributor.hpp>
#include <network_worker.hpp>
#include <ob1_payload_decoder.hpp>
#include <sentry_payload_decoder.hpp>

using namespace Workers;
/**
 * @brief Wrapper over the control worker that implements the Qt methods.
 * Provides the glue logic between the Qt world and the scimon specifics.
 */
class ControlWorkerWrapper : public QObject {
  Q_OBJECT

public:
  ControlWorkerWrapper(int messageDistributorInterval,
                       QObject *parent = Q_NULLPTR);

  ~ControlWorkerWrapper();

  /**
   * @brief Queries some states of the worker periodically, and emits signals
   * accordingly.
   */
  void periodicActionsWorker();

public slots:
  /**
   * @brief Starts querying of the states of the underlying worker.
   */
  void startStateQuery();

  /**
   * @brief Shuts down the Control worker.
   */
  void shutdown();

  /**
   * @brief Initiates the connection to the given host.
   * @param ip The IP to which a connection shall be established.
   * @param port The port to which a connection shall be established.
   */
  void startConnection(QString ip, int port);

  void startConfig();

  /**
   * @brief Sends a start command to the underlying worker.
   */
  void start();

  /**
   * @brief Sends s stop command to the underlying worker.
   */
  void stop();

  void start_isx3();

  void stop_isx3();

  void start_ob1();

  void stop_ob1();

  void setPressure(int channel, double pressure);

  /**
   * @brief Converts the given enum value to a QString.
   * @param subState The enum value that shall be converted to a string.
   * @return String that represents the enum value of subState.
   */
  static QString controlWorkerSubStateToString(ControlWorkerSubState subState);

signals:
  void stateChanged(DeviceStatus oldState, DeviceStatus newState);
  void subStateChanged(ControlWorkerSubState oldState,
                       ControlWorkerSubState newState);
  void remoteStateChanged(QList<std::shared_ptr<StatusPayload>> oldStates,
                          QList<std::shared_ptr<StatusPayload>> newStates);
  void newSpectrumData(
      const std::vector<std::tuple<TimePoint, ImpedanceSpectrum>> &data);

private:
  std::shared_ptr<MessageDistributor> messageDistributor;
  std::shared_ptr<NetworkWorker> networkWorker;
  std::shared_ptr<ControlWorker> controlWorker;
  std::unique_ptr<std::thread> messageThread;
  std::unique_ptr<std::thread> periodicActionsThread;
  bool doPeriodicActions;

  /// The timestamp of the most recent spectrum query.
  TimePoint recentSpectrumQueryTimestamp;
};

#endif
