/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TMW_MONSTER_H
#define _TMW_MONSTER_H

#include "being.h"

class MonsterInfo;

class Monster : public Being
{
    public:
        Monster(Uint16 id, Uint16 job, Map *map);

        virtual ~Monster();

        virtual void setAction(Action action, int attackType);

        virtual Type getType() const;

        virtual TargetCursorSize
        getTargetCursorSize() const;

        /**
         * Handles an attack of another being by this monster. Plays a hit or
         * miss sound when appropriate.
         */
        virtual void handleAttack();

        /**
         * Puts a damage bubble above this monster and plays the hurt sound
         *
         * @param amount The amount of damage.
         */
        virtual void takeDamage(int amount);

        /**
         * Returns the MonsterInfo, with static data about this monster.
         */
        const MonsterInfo&
        getInfo() const;

        /**
         * Gets the way the monster is blocked by other objects
         */
        virtual unsigned char getWalkMask() const
        { return 0x83; } // blocked by walls, other monsters and players ( bin 1000 0011)

    protected:
        /**
         * Gets the way the monster blocks pathfinding for other objects
         */
        virtual Map::BlockType getBlockType() const
        { return Map::BLOCKTYPE_MONSTER; }
};

#endif
