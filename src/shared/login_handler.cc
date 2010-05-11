/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#include <string>
#include "login_handler.h"
#include "session.h"
#include "packet_tools.h"

namespace sandbox {
namespace shared {

void HandleAuthentication(Session& session, sandbox::shared::ByteBuffer& message)
{
    message;
	// Send the station identifier
	session.sendHardcodedPacket("packets/Login/StationIdentifier.txt", false);

	// Send the server list.
	session.sendHardcodedPacket("packets/Login/ServerList.txt", false);

    std::tr1::shared_ptr<ByteBuffer> packet = LoadPacketFromTextFile("packets/Login/ServerIpList.txt");
    
    std::string galaxy_ip = "127.0.0.1"; // @todo: Replace with a call to configuration.
    
    // Write the galaxy ip string length.
    packet->writeAt<uint16_t>(18, static_cast<uint16_t>(galaxy_ip.length()));    
    
    // Insert the galaxy ip string into the packet data.
    std::vector<uint8_t>& packet_data = packet->raw();  
    packet_data.insert(packet_data.begin() + 20, galaxy_ip.begin(), galaxy_ip.end());
    
    session.sendHardcodedPacket(*packet, false);

	// Send the character list.
	session.sendHardcodedPacket("packets/Login/ServerCharacterList.txt", false);
}

void HandleSession(Session& session, sandbox::shared::ByteBuffer& message)
{
    message;
	session.sendHeartbeat();

	// Send the character options to the client.
	session.sendHardcodedPacket("packets/ZoneInsertion/CharacterOptions.txt", false);
}

}  // namespace sandbox
}  // namespace shared
