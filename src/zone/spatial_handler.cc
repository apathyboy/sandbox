/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include "spatial_handler.h"
#include "shared/session.h"
#include "shared/logger.h"
#include "shared/packet_tools.h"

namespace sandbox {
namespace zone {

void HandleSpatial(shared::Session& session, shared::ByteBuffer& message)
{	
    uint32_t opcode = message.peekAt<uint32_t>(30);
    //unsigned int *opcode = (unsigned int*)(data+30);
	// Try to handle the incoming packet.
	try
	{
		// Search for the opcode handler function and pass it the packet data.
	//	MessageHandler handler = OpcodeFactory::getOpcodeHandler(opcode);
	//	handler(session, message);
	}
	catch(...)
	{
		// Log any unknown opcodes.
    shared::Logger().log(shared::ERR) << "Unknown Spatial Opcode Found: " << opcode << std::endl << message;	
		session.sendHeartbeat();
	}
}


void HandlePositionUpdate(shared::Session& session, shared::ByteBuffer& message)
{
    // @TODO: Write this.
}


void HandleSit(shared::Session& session, shared::ByteBuffer& message)
{
	session.sendHeartbeat();

	session.sendHardcodedPacket("packets/Spatial/SelfPostureUpdate_Sit.txt", false);
	session.sendHardcodedPacket("packets/Spatial/PostureUpdate_Sit.txt", false);
}


void HandleStand(shared::Session& session, shared::ByteBuffer& message)
{
	session.sendHeartbeat();

	session.sendHardcodedPacket("packets/Spatial/SelfPostureUpdate_Stand.txt", false);
	session.sendHardcodedPacket("packets/Spatial/PostureUpdate_Stand.txt", false);
}


void HandleProne(shared::Session& session, shared::ByteBuffer& message)
{
	session.sendHeartbeat();

	session.sendHardcodedPacket("packets/Spatial/SelfPostureUpdate_Prone.txt", false);
	session.sendHardcodedPacket("packets/Spatial/PostureUpdate_Prone.txt",false);
}


void HandleKneel(shared::Session& session, shared::ByteBuffer& message)
{
	session.sendHeartbeat();

	session.sendHardcodedPacket("packets/Spatial/SelfPostureUpdate_Kneel.txt", false);
	session.sendHardcodedPacket("packets/Spatial/PostureUpdate_Kneel.txt",false);

}


void HandleSpatialChat(shared::Session& session, shared::ByteBuffer& message)
{
    shared::Logger().log(shared::INFO) << "Spatial Chat Packet" << std::endl << message;
    session.sendHeartbeat();

    uint32_t textsize = message.peekAt<uint32_t>(42);
    message.readPosition(46);

    std::vector<int8_t> moodString(32, 0);
    std::vector<uint64_t> mood(5, 0);
    uint8_t moodLength = 0;

    int8_t moodPosition = 0;
	int8_t j = 0;
    while (moodPosition < 5) {
        int8_t current = message.read<uint8_t>();

        if (current == 32) {
            ++moodLength;
            message.read<int8_t>();

            mood[moodPosition] = atoi(reinterpret_cast<char *>(&moodString[0]));
            moodString.empty();
            ++moodPosition;
	        j = 0;
            continue;
        }   

        if (current == 0) {
            continue;
        }

        ++moodLength;
        moodString[j] = current;
    }

	if (mood[2] == 0)
		mood[2] = (uint16_t)session.player()->mood();

    std::vector<uint8_t>& packet_data = message.raw();

    while (message.readPosition() + ((textsize-10) * 2) >= message.size()) {
        message << 0x00;
    }

    std::wstring text(
        reinterpret_cast<const wchar_t*>(&packet_data[message.readPosition()]), 
        reinterpret_cast<const wchar_t*>(&packet_data[message.readPosition() + ((textsize-moodLength) * 2)]));
    session.sendText(text, mood);
}


void HandleMood(shared::Session& session, shared::ByteBuffer& message)
{
    shared::Logger().log(shared::INFO) << "Mood Packet" << std::endl << message;
	session.sendHeartbeat();

    int32_t size = message.peekAt<uint32_t>(42);
    message.readPosition(46);

    std::vector<int8_t> mood(size+1, 0);

    for (int32_t i = 0; i < size; ++i) {
        if (message.peek<int8_t>() == 0 || message.peek<int8_t>() == 32) break;

        mood[i] = message.read<int8_t>();
        message.read<int8_t>();
    }

    session.player()->mood(atoi(reinterpret_cast<char *>(&mood[0])));

    std::tr1::shared_ptr<shared::ByteBuffer> packet = shared::LoadPacketFromTextFile("packets/ZoneInsertion/Creo6.txt");

    // Insert the player mood into the packet.
    packet->writeAt<uint16_t>(99, static_cast<uint16_t>(session.player()->mood()));
    session.sendHardcodedPacket(*packet, true);
}


void HandleEmote(shared::Session& session, shared::ByteBuffer& message)
{
    shared::Logger().log(shared::INFO) << "Emote Packet" << std::endl << message;
	session.sendHeartbeat();

    uint32_t size = message.peekAt<uint32_t>(42);
    message.readPosition(46);

    message.read<uint32_t>();

    std::vector<int8_t> emote(size);

    for (uint32_t i = 0; i < size; ++i) {
        if (message.peek<int8_t>() == 0 || message.peek<int8_t>() == 32) break;

        emote[i] = message.read<int8_t>();
        message.read<int8_t>();
    }

	uint16_t emoteId = atoi(reinterpret_cast<char *>(&emote[0]));
    
    std::tr1::shared_ptr<shared::ByteBuffer> packet = shared::LoadPacketFromTextFile("packets/Spatial/PlayerEmote.txt");

    // Insert the player mood into the packet.
    packet->writeAt<uint16_t>(46, emoteId);
    session.sendHardcodedPacket(*packet, true);
}

}  // namespace zone
}  // namespace sandbox
