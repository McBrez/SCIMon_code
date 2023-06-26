#ifndef DATA_MANAGER
#define DATA_MANAGER

// Standard includes
#include <map>
#include <string>
#include <variant>

// Project includes
#include <utilities.hpp>

namespace Utilities {

/**
 * @brief The type of the data manager.
 */
enum DataManagerType {
  /// Invalid data manager type
  DATAMANAGER_TYPE_INVALID = 0x00,
  /// Data manager with HDF backend.
  DATAMANAGER_TYPE_HDF = 0x01,
};

/**
 * @brief Identifies the supported data types.
 */
enum DataManagerDataType {
  DATAMANAGER_DATA_TYPE_INVALID = 0x00,

  DATAMANAGER_DATA_TYPE_INT = 0x01,

  DATAMANAGER_DATA_TYPE_DOUBLE = 0x02,

  DATAMANAGER_DATA_TYPE_COMPLEX = 0x03,

  DATAMANAGER_DATA_TYPE_STRING = 0x04
};

/// @brief Shortcut to a type that defines a mapping between a a data key name
/// and the data type.
typedef std::map<std::string, DataManagerDataType> KeyMapping;
/// Used to pass values to the data manager.
typedef std::variant<int, double, std::complex<double>, std::string> Value;

/**
 * @brief Class interface to a class that manages data read and write operations
 * to persistant storage.
 */
class DataManager {
public:
  /**
   * @brief Destroy the Data Manager object
   */
  virtual ~DataManager() = 0;

  /**
   * @brief Queries the data manager with the given timestamp and key.
   *
   * @param timestamp The timestamp that shall be queried.
   * @param key The key that shall be queried.
   * @param value WIll contain the value.
   * @return TRUE if data has been retrieved succesfully. FALSE otherwise.
   */
  virtual bool read(TimePoint timestamp, std::string key, Value &value) = 0;

  /**
   * @brief Queries the data manager with the given time frame and key.
   *
   * @param from The start of the time frame, that shall be queried.
   * @param to The end of the time frame, that shall be queried.
   * @param key The key that shall be queried.
   * @param value Will contain the value.
   * @return TRUE if data has been retrieved succesfully. FALSE otherwise.
   */
  virtual bool read(TimePoint from, TimePoint to, std::string key,
                    Value &value) = 0;

  /**
   * @brief Writes the given data to the underlying data base.
   *
   * @param timestamp The timestamp that shall be stored with the given data.
   * @param key The under which the data shall be stored.
   * @param value The value that shall be stored.
   * @return TRUE if write operation was succesfull. False otherwise.
   */
  virtual bool write(TimePoint timestamp, std::string key,
                     const Value &value) = 0;

  /**
   * @brief Tries to open an already existing data base.
   *F
   * @param name The name of the data base, that shall be opened.
   * @return TRUE if a database has been opened. False otherwise.
   */
  virtual bool open(std::string name) = 0;

  /**
   * @brief Creates a new database and opens it.
   *
   * @param name The name of the database, that shall be opened.
   * @param keyMapping The Key mapping that shall be used.
   * @param force Specifies, whether an already existing database, with the same
   * name, shall be overwritten.
   * @return TRUE if a dataabse has been opened. False otherwise.
   */
  virtual bool open(std::string name, KeyMapping keyMapping,
                    bool force = false) = 0;

  /**
   * @brief Closes the connection to the underlying database.
   *
   * @return TRUE if underlying data base has been closed successfully.
   * FALSE otherwise.
   */
  virtual bool close() = 0;

  /**
   * @brief Whether the underlying data base is open and the data manager is
   * operational.
   *
   * @return TRUE if data manager is operational. FALSE otherwise.
   */
  bool isOpen() const;

  /**
   * @brief Returns the key mapping.
   * @return The key mapping.
   */
  KeyMapping getKeyMapping() const;

  /**
   * @brief Returns the type of the data manager.
   * @return The data manager type.
   */
  virtual DataManagerType getDataManagerType() const = 0;

  /**
   * @brief Returns a data manager with the given type.
   * @param dataManagerType The type of the data manager.
   * @return Pointer to the data manager. May return nullptr, if the given type
   * is not available.
   */
  static std::shared_ptr<DataManager>
  getDataManager(DataManagerType dataManagerType);

protected:
  /// Flag that indicates, whether the underlying data base has been opened.
  bool openFlag;
  /// Holds the mapping betwenn key and data type.
  KeyMapping typeMapping;
};
} // namespace Utilities

#endif
