#ifndef USER_ID_HPP
#define USER_ID_HPP

namespace Messages {

class MessageInterface;

/**
 * @brief Encapsulate an user id.
 */
class UserId {
public:
  /**
   * @brief Constructs an invalid user id.
   */
  UserId();

  /**
   * @brief Creates an user id with an automatically generated id.
   * @param Pointer to the object that shall be identified by this id.
   */
  UserId(MessageInterface *messageInterface);

  /**
   * @brief Copy constructor.
   * @param other The UserId that shall be copied.
   */
  UserId(const UserId &other);

  /**
   * @brief Creates an device id with the given id.
   * @param userId: The id of the object.
   */
  UserId(size_t UserId);

  /**
   * @brief Returns the id.
   * @return The id.
   */
  size_t id() const;

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

  operator bool() const;

  bool isValid() const;

private:
  /// @brief The unique user id.
  size_t userId;

  /**
   * @brief Generates an unique user id.
   * @param messageInterface Pointer to the object this id shall represent.
   * @return A unique user id.
   */
  size_t generateUserId(MessageInterface *messageInterface);
};
} // namespace Messages

#endif