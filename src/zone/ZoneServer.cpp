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

#include "ZoneServer.h"
#include "Logger.h"
#include "GalaxySession.h"

ZoneServer::ZoneServer()
: SocketServer()
{}

ZoneServer::ZoneServer(uint16_t port)
: SocketServer(port)
{}

ZoneServer::~ZoneServer()
{}

void ZoneServer::InitServer()
{
	// Store the config file and load some of the data that
	// will be needed.
	SocketServer::InitServer();
}

void ZoneServer::Run()
{	
	Logger().log(INFO) << "Zone server listening on port [" << port() << "]";
	SocketServer::Run();
}

void ZoneServer::OnUpdate()
{	
	static time_t lastCleanupTime = mCurrentTime;

	if ((mCurrentTime - lastCleanupTime) >= 1) {
		for (GalaxySessionMap::iterator i = mSessions.begin();
			 i != mSessions.end();
			 i++)
		{
            std::tr1::shared_ptr<GalaxySession> session = (*i).second;
			session->Update(mCurrentTime);
		}
	}

	lastCleanupTime = mCurrentTime;
}