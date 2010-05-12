/**
 * SWG:ANH Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
 */

#ifndef SRC_SHARED_BYTE_BUFFER_INL_H_
#define SRC_SHARED_BYTE_BUFFER_INL_H_

#include <string>

namespace sandbox {
namespace shared {

template<typename T>
void ByteBuffer::swapEndian(T& data) {
  data; /* Only template specializations of swapEndian should be used */
}

template<typename T>
ByteBuffer& ByteBuffer::write(T data) {
  write(reinterpret_cast<unsigned char*>(&data), sizeof(T));
  return *this;
}

template<typename T>
ByteBuffer& ByteBuffer::writeAt(size_t offset, T data) {
  write(offset, reinterpret_cast<unsigned char*>(&data), sizeof(T));
  return *this;
}

template<typename T>
const T ByteBuffer::read(bool doSwapEndian) {
  T data = peek<T>(doSwapEndian);
  read_position_ += sizeof(T);
  return data;
}

template<typename T>
const T ByteBuffer::peek(bool doSwapEndian) {
  return peekAt<T>(read_position_, doSwapEndian);
}

template<typename T>
const T ByteBuffer::peekAt(size_t offset, bool doSwapEndian) {
  if (data_.size() < offset + sizeof(T)) {
    throw std::out_of_range("Read past end of buffer");
  }

  T data = *reinterpret_cast<T*>(&data_[offset]);

  if (doSwapEndian)
    swapEndian<T>(data);

  return data;
}

template<typename T>
ByteBuffer& operator<<(ByteBuffer& buffer, const T& value) {
  buffer.write<T>(value);
  return buffer;
}

template<> void ByteBuffer::swapEndian(uint16_t& data);
template<> void ByteBuffer::swapEndian(uint32_t& data);
template<> void ByteBuffer::swapEndian(uint64_t& data);

template<> ByteBuffer& ByteBuffer::write<std::string>(std::string data);
template<> const std::string ByteBuffer::read<std::string>(bool doSwapEndian);
template<> ByteBuffer& ByteBuffer::write<std::wstring>(std::wstring data);
template<> const std::wstring ByteBuffer::read<std::wstring>(bool doSwapEndian);

}  // namespace shared
}  // namespace sandbox

#endif  // SRC_SHARED_BYTE_BUFFER_INL_H_
