/*
 * OpenSWG Combat Upgrade Sandbox - Player.cpp
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#include "Player.h"

Player::Player()
    : position_()
    , station_id_(0)
    , mood_id_(0)
{}

const Vector3<int>& Player::position() const
{
    return position_;
}

void Player::position(const Vector3<int>& position)
{
    position_ = position;
}

const std::string& Player::stationName() const
{
    return station_name_;
}

void Player::stationName(const std::string& name)
{
    station_name_ = name;
}

const std::string& Player::locationName() const
{
    return location_name_;
}

void Player::locationName(const std::string& name)
{
    location_name_ = name;
}

uint8_t Player::mood() const
{ 
    return mood_id_; 
}

void Player::mood(uint8_t moodId) 
{ 
    mood_id_ = moodId; 
}

int Player::stationId() const
{
    return station_id_;
}

void Player::stationId(int id)
{
    station_id_ = id;
}

