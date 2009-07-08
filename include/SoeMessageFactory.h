/*
 * OpenSWG: GalaxyEngine - SoeMessageFactory.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef SOE_MESSAGE_FACTORY_H_
#define SOE_MESSAGE_FACTORY_H_

#include <tr1/memory>

class ByteBuffer;
class Session;

class SoeMessageFactory
{
public:
    static std::tr1::shared_ptr<ByteBuffer> buildSessionResponse(std::tr1::shared_ptr<Session>);

private:
	SoeMessageFactory();
	~SoeMessageFactory();
    SoeMessageFactory(const SoeMessageFactory&);
    SoeMessageFactory& operator=(const SoeMessageFactory&);
};

#endif // SOE_MESSAGE_FACTORY_H_

