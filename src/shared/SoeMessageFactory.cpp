/*
 * OpenSWG: GalaxyEngine - SoeMessageFactory.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "SoeMessageFactory.h"
#include "ByteBuffer.h"

std::tr1::shared_ptr<ByteBuffer> SoeMessageFactory::buildSessionResponse(std::tr1::shared_ptr<Session>)
{
    std::tr1::shared_ptr<ByteBuffer> buffer(new ByteBuffer());
    return buffer;
}