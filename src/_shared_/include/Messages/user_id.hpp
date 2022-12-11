#ifndef USER_ID_HPP
#define USER_ID_HPP

namespace Messages {
/**
 * @brief Encapsulate an user id.
 */
class UserId {
public:
  /**
   * @brief Creates an user id with an automatically generated id.
   */
  UserId();

  /**
   * @brief Creates an device id with the given id.
   * @param userId: The id of the object.
   */
  UserId(int UserId);

  /**
   * @brief Returns the id.
   * @return The id.
   */
  int id() const;

  /**
   * @brief Checks if two device ids are equal.
   *
   * @param other The other device id.
   * @return True if device ids are equal. False otherwise.
   */
  bool operator==(const UserId &other);

  /**
   * @brief Checks if two device ids are unequal.
   *
   * @param other The other device id.
   * @return True if device ids are unequal. False otherwise.
   */
  bool operator!=(const UserId &other);

private:
  /// @brief The unique user id.
  int userId;

  /**
   * @brief Generates an unique user id.
   * @return A unique user id.
   */
  int generateUserId();
};
} // namespace Messages

#endif