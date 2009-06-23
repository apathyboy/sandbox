/**
 * *********************************************************************
 * OpenSWG Sandbox Server
 * Copyright (C) 2006 OpenSWG Team <http://www.openswg.com>
 * *********************************************************************
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * To read the license please visit http://www.gnu.org/copyleft/gpl.html
 * *********************************************************************
 */

#include "ZoneInsertionHandler.h"
#include "GalaxySession.h"
#include "PacketTools.h"


void HandleClientReady(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();
}

void HandleLoadDone(GalaxySession *session, char *data, unsigned short length)
{
	session->SendHardPacket("packets\\ZoneInsertion\\LoadDone.txt", true);
}

void HandleLoadTerrain(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();
}

void HandleSession(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	// Send the character options to the client.
	session->SendHardPacket("packets\\ZoneInsertion\\CharacterOptions.txt", false);
}

void HandleZoneInsertionRequest(GalaxySession *session, char *data, unsigned short length)
{
	session->SendOk();

	// Send the initial batch of packets.
	session->SendHardPacket("packets\\ZoneInsertion\\UnkByteFlag.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\Int900Pkt.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\LoadTerrain.txt", true);

	// Begin loading the character.
	session->SendHardPacket("packets\\ZoneInsertion\\PreloadCharacter.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\CreoCreate.txt", false);
	//session->SendHardPacket("packets\\ZoneInsertion\\PrecloseClose.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\Creo3.txt", true);
	session->SendHardPacket("packets\\ZoneInsertion\\Creo6.txt", true);

	session->SendHardPacket("packets\\ZoneInsertion\\Creo1.txt", true);

	// @TODO Implement a better way to send hardcoded fragmented packets.
	session->SendHardPacket("packets\\ZoneInsertion\\Creo4.txt", true);
	session->SendHardPacket("packets\\ZoneInsertion\\Creo4pt2.txt", true);
	session->SendHardPacket("packets\\ZoneInsertion\\Creo4pt3.txt", true);
	session->SendHardPacket("packets\\ZoneInsertion\\Creo4pt4.txt", true);

	// Create our character.
	session->SendHardPacket("packets\\ZoneInsertion\\Creo8.txt", true);
	session->SendHardPacket("packets\\ZoneInsertion\\Play8.txt", true);
	session->SendHardPacket("packets\\ZoneInsertion\\Play9.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\PrecloseClose.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\PlayClose.txt", false);
	session->SendHardPacket("packets\\ZoneInsertion\\Play9Update.txt", false);

	// Inventory
	session->SendHardPacket("packets\\ZoneInsertion\\Inventory.txt", false);

	// Environment
	session->SendHardPacket("packets\\Objects\\KerenStarship.txt", false);
	session->SendHardPacket("packets\\Objects\\KerenStarshipTicketCollector.txt", false);
}