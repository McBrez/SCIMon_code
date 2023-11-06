#ifndef DATA_MANAGER_HDF
#define DATA_MANAGER_HDF

// Standard includes
#include <memory>

// 3rd party includes
#include <highfive/H5File.hpp>

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
  virtual bool read(TimePoint timestamp, const std::string &key,
                    Value &value) override;

  /**
   * @brief Queries the data manager with the given time frame and key.
   *
   * @param from The start of the time frame, that shall be queried.
   * @param to The end of the time frame, that shall be queried.
   * @param key The key that shall be queried.
   * @param timestamps Will contain the timestamps that correspond to the
   * values..
   * @param value Will contain the value.
   * @return TRUE if data has been retrieved succesfully. FALSE otherwise.
   */
  virtual bool read(TimePoint from, TimePoint to, const std::string &key,
                    std::vector<TimePoint> &timestamps,
                    std::vector<Value> &value) override;

  /**
   * @brief Writes the given data to the underlying data base.
   *
   * @param timestamp The timestamp that shall be stored with the given data.
   * @param key The under which the data shall be stored.
   * @param value The value that shall be stored.
   * @return TRUE if write operation was succesfull. False otherwise.
   */
  virtual bool write(TimePoint timestamp, const std::string &key,
                     const Value &value) override;

  /**
   * @brief Writes the given data to the underlying data base.
   *
   * @param timestamp The timestamp that shall be stored with the given data.
   * @param key The under which the data shall be stored.
   * @param value The value that shall be stored.
   * @return TRUE if write operation was succesfull. False otherwise.
   */
  virtual bool write(const std::vector<TimePoint> &timestamp,
                     const std::string &key,
                     const std::vector<Value> &value) override;

  /**
   * @brief Tries to open an already existing data base.
   *F
   * @param name The name of the data base, that shall be opened (with file
   * extension).
   * @return TRUE if a database has been opened. False otherwise.
   */
  virtual bool open(std::string name) override;

  /**
   * @brief Creates a new database and opens it.
   *
   * @param name The name of the database, that shall be opened (without file
   * extension).
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
   * @return The data manager type.   */
  virtual DataManagerType getDataManagerType() const override;

  /**
   * @brief Creates a key with the given name and data type.
   * @param key The name of the key.
   * @param dataType The datatype this key is associated with.
   * @return Whether the creation of the key succeded.
   */
  virtual bool createKey(std::string key,
                         DataManagerDataType dataType) override;

  /**
   * @brief Returns the timerange mapping of the data manager. The timerange
   * mapping contains the oldest and the most recent timestamp per data key.
   * @return The timerange mapping at the time this method is called.
   */
  virtual TimerangeMapping getTimerangeMapping() const override;

protected:
  /**
   * @brief Sets up the details of a spectrum.
   * @return TRUE if setup was successfull. False otherwise.
   */
  virtual bool setupSpectrumSpecific(std::string key,
                                     std::vector<double> frequencies) override;

private:
  /**
   * @brief Extends the given dataset by the given count of elements.
   * @param name The name of the dataset that shall be extended.
   * @param count The count of elements the dataset shall be extended by.
   * @return The size of the extended dataset.
   */
  size_t extendDataSet(const std::string &name, int count);

  /**
   * @brief Helper, that writes the datum to the given dataset and automatically
   * extends the datasets.
   * @param timestamp The timestamp of the datum.
   * @param key The key of the datum.
   * @param value the value of the datum.
   * @return TRUE if write was succesfull. FALSE otherwise.
   */
  bool extendingWrite(const std::vector<TimePoint> &timestamp,
                      const std::string &key, const std::vector<Value> &value);

  /**
   * @brief Transforms the value vector into a more specific form.
   * @param origVec The Vector that shall be transformed.
   * @param transformedVec Will contain the transformed vector.
   * @return TRUE if transformation was succesfull. FALSE otherwise.
   */
  template <class T>
  bool transformValueVector(const std::vector<Value> &origVec,
                            std::vector<T> &transformedVec);

  /**
   * @brief Transforms the timestamp vector into a vector that can more easily
   * be written to a file.
   * @param origVec The Vector that shall be transformed.
   * @param transformedVec Will contain the transformed vector.
   * @return TRUE if transformation was succesfull. FALSE otherwise.
   */
  bool transformTimestampVector(const std::vector<TimePoint> &origVec,
                                std::vector<long long> &transformedVec);

  /// Pointer to the file.
  std::unique_ptr<HighFive::File> hdfFile;

  /// The default chunking size.
  const hsize_t defaultChunkingSize = 1024;

  /// Guard for reads and writes to the data manager. It has to be static, as
  /// HD5 does not allow to read/write to multiple files simultaneously.
  static std::mutex dataManagerMutex;
};
} // namespace Utilities

#endif
