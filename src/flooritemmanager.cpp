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
 *
 *  $Id$
 */

#include <algorithm>

#include "flooritemmanager.h"

#include "floor_item.h"

#include "utils/dtor.h"

FloorItemManager::~FloorItemManager()
{
    clear();
}

FloorItem* FloorItemManager::create(unsigned int id, unsigned int itemId,
        unsigned short x, unsigned short y, Map *map)
{
    FloorItem *floorItem = new FloorItem(id, itemId, x, y, map);
    mFloorItems.push_back(floorItem);
    return floorItem;
}

void FloorItemManager::destroy(FloorItem *item)
{
    mFloorItems.remove(item);
    delete item;
}

void FloorItemManager::clear()
{
    for_each(mFloorItems.begin(), mFloorItems.end(), make_dtor(mFloorItems));
    mFloorItems.clear();
}

FloorItem* FloorItemManager::findById(unsigned int id)
{
    FloorItemIterator i;
    for (i = mFloorItems.begin(); i != mFloorItems.end(); i++)
    {
        if ((*i)->getId() == id)
        {
            return *i;
        }
    }

    return NULL;
}

FloorItem* FloorItemManager::findByCoordinates(unsigned short x,
        unsigned short y)
{
    FloorItemIterator i;
    for (i = mFloorItems.begin(); i != mFloorItems.end(); i++)
    {
        if ((*i)->getX() == x && (*i)->getY() == y)
        {
            return *i;
        }
    }

    return NULL;
}
