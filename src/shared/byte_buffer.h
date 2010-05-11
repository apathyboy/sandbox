/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef BYTEBUFFER_H_
#define BYTEBUFFER_H_

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

namespace sandbox {
namespace shared {

class ByteBuffer;
  
template<typename T> 
ByteBuffer& operator<<(ByteBuffer& buffer, const T& value);

std::ostream& operator<<(std::ostream& message, const ByteBuffer& buffer);

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

	void swap(ByteBuffer& from);

    void append(const ByteBuffer& from);

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

}  // namespace shared
}  // namespace sandbox

// Move inline implementations to a separate file to clean up the declaration header.
#include "byte_buffer-inl.h"

#endif /* BYTEBUFFER_H_ */
