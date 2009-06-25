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

#include "Player.h"

Player::Player()
    : position_()
    , mMoodId(0)
{}

const Vector3<int>& Player::position() const
{
    return position_;
}

void Player::position(const Vector3<int>& position)
{
    position_ = position;
}

void Player::ToggleInitialized() { 
	if (mInitialized)
		mInitialized = false;
	else
		mInitialized = true;
};
