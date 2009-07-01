/*
 * OpenSWG Combat Upgrade Sandbox - OpcodeFactory.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef OPENSWG_OPCODE_FACTORY_H
#define OPENSWG_OPCODE_FACTORY_H

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <map>
#include <tr1/functional>

/** Exception that is thrown by the OpcodeFactory class. Client code
 *	using the opcode handler should check to see if this exception is
 *	being thrown.
 */
struct OpcodeHandlerException
{};

// Forward declaration of Session needed b the opcode handlers.
class Session;
class ByteBuffer;

typedef std::tr1::function<void (Session&, std::tr1::shared_ptr<ByteBuffer>)> MessageHandler;
typedef std::map<uint32_t, MessageHandler> MessageHandlers;


/** Opcode Factory
 *  This is a factory class that creates and returns opcode handlers.
 */
class OpcodeFactory
{
public:
	static MessageHandler getOpcodeHandler(std::tr1::shared_ptr<ByteBuffer> packet);
	static MessageHandler getOpcodeHandler(uint32_t opcode);

private:
	static MessageHandlers _buildMessageHandlerMap();
};

#endif // OPENSWG_OPCODE_FACTORY_H