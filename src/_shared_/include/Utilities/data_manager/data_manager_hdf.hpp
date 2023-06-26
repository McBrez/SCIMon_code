#ifndef DATA_MANAGER_HDF
#define DATA_MANAGER_HDF

// Project includes
#include <data_manager.hpp>

namespace Utilities {
class DataManagerHdf : public DataManager {
public:
   /**
   * @brief Destroy the Data Manager object
   */
    virtual ~DataManagerHdf() override;

    /**
   * @brief Queries the data manager with the given timestamp and key.
   *
   * @param timestamp The timestamp that shall be queried.
   * @param key The key that shall be queried.
   * @param value WIll contain the value.
   * @return TRUE if data has been retrieved succesfully. FALSE otherwise.
   */
    virtual bool read(TimePoint timestamp, std::string key,
                      Value &value) override;

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
                      Value &value) override;

    /**
   * @brief Writes the given data to the underlying data base.
   *
   * @param timestamp The timestamp that shall be stored with the given data.
   * @param key The under which the data shall be stored.
   * @param value The value that shall be stored.
   * @return TRUE if write operation was succesfull. False otherwise.
   */
    virtual bool write(TimePoint timestamp, std::string key,
                       const Value &value) override;

    /**
   * @brief Tries to open an already existing data base.
   *F
   * @param name The name of the data base, that shall be opened.
   * @return TRUE if a database has been opened. False otherwise.
   */
    virtual bool open(std::string name) override;

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
                      bool force = false) override;

    /**
   * @brief Closes the connection to the underlying database.
   *
   * @return TRUE if underlying data base has been closed successfully.
   * FALSE otherwise.
   */
    virtual bool close() override;

    /**
   * @brief Returns the type of the data manager.
   * @return The data manager type.
   */
      virtual DataManagerType getDataManagerType() const override;

};
} // namespace Utilities

#endif
