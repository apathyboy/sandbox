/*
 * Legacy Sandbox Project - ByteBuffer.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/
#ifndef BYTEBUFFER_H_
#define BYTEBUFFER_H_

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <vector>
#include <string>
#include <stdexcept>

class ByteBuffer;

template<typename T> 
ByteBuffer&   operator<< (ByteBuffer& buffer, const T& value);

std::ostream& operator<< (std::ostream& message, const ByteBuffer& buffer);

class ByteBuffer
{		
public:
	enum { SWAP_ENDIAN = 1 };

public:
	ByteBuffer  		();
	explicit ByteBuffer (size_t length);
	explicit ByteBuffer (std::vector<unsigned char>& data);
	ByteBuffer 			(const unsigned char* data, size_t length);
	~ByteBuffer 		();

	ByteBuffer			      (const ByteBuffer& from);
	ByteBuffer& operator= (const ByteBuffer& from);

	void swap (ByteBuffer& from);

	template<typename T> ByteBuffer& write (T data);
	template<typename T> ByteBuffer& writeAt (size_t offset, T data);
	template<typename T> const T     peek  (bool doSwapEndian = false);
	template<typename T> const T     peekAt  (size_t offset, bool doSwapEndian = false);
	template<typename T> const T 	 read  (bool doSwapEndian = false);

	void write (const unsigned char* data, size_t size);
	void write (size_t offset, const unsigned char* data, size_t size);
	void clear ();

    size_t readPosition() const;
    void   readPosition(size_t position);

    size_t writePosition() const;
    void   writePosition(size_t position);

    void                 reserve  (size_t length);
	size_t               size     () const;
	size_t               capacity () const;
	const unsigned char* data     () const;

    std::vector<unsigned char>&  raw();

private:
	template<typename T> void swapEndian(T& data);

	std::vector<unsigned char> data_;
	size_t                     read_position_;
	size_t                     write_position_;

}; // ByteBuffer

template<typename T> void ByteBuffer::swapEndian(T& data)
{ /* Only template specializations of swapEndian should be used */ }

template<typename T> ByteBuffer& ByteBuffer::write(T data)
{
	write(reinterpret_cast<unsigned char*>(&data), sizeof(T));
	return *this;
}

template<typename T> ByteBuffer& ByteBuffer::writeAt(size_t offset, T data)
{
	write(offset, reinterpret_cast<unsigned char*>(&data), sizeof(T));
	return *this;
}


template<typename T> const T ByteBuffer::read(bool doSwapEndian)
{
	T data = peek<T>(doSwapEndian);
	read_position_ += sizeof(T);
	return data;
}

template<typename T> const T ByteBuffer::peek(bool doSwapEndian)
{
	return peekAt<T>(read_position_, doSwapEndian);
}

template<typename T> const T ByteBuffer::peekAt(size_t offset, bool doSwapEndian)
{
	if (data_.size() < offset + sizeof(T)) {
		throw std::out_of_range("Read past end of buffer");
	}

	T data = *reinterpret_cast<T*>(&data_[offset]);

	if (doSwapEndian)
		swapEndian<T>(data);

	return data;
}

template<typename T> ByteBuffer& operator<<(ByteBuffer& buffer, const T& value)
{
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

#endif /* BYTEBUFFER_H_ */
