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

#ifndef OPENSWG_PLAYER_H
#define OPENSWG_PLAYER_H

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <cstdint>
#endif

#include <string>

#include "Vector3.h"

class Player
{
public:
	Player();

    const Vector3<int>& position() const;
    void position(const Vector3<int>& position);

    const std::string& stationName() const;
    void stationName(const std::string& name);

    const std::string& locationName() const;
    void locationName(const std::string& name);

    uint8_t mood() const;
    void mood(uint8_t moodId);

    int stationId() const;
    void stationId(int id);

private:
    Player(const Player&);
    Player& operator=(const Player&);

    Vector3<int> position_;
    std::string  station_name_;
    std::string  location_name_;
    uint8_t      mood_id_;
    int          station_id_;
};

#endif // OPENSWG_PLAYER_H