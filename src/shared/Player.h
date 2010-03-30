/*
 * SWG:ANH Combat Upgrade Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
**/

#ifndef SRC_SHARED_PLAYER_H_
#define SRC_SHARED_PLAYER_H_

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

#endif // SRC_SHARED_PLAYER_H_