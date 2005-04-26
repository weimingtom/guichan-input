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

#ifndef _TMW_MAP_H
#define _TMW_MAP_H

#include "being.h"
#include "graphics.h"
#include "resources/image.h"
#include <list>

/**
 * A meta tile stores additional information about a location on a tile map.
 * This is information that doesn't need to be repeated for each tile in each
 * layer of the map.
 */
class MetaTile
{
    public:
        /**
         * Constructor.
         */
        MetaTile();

        // Pathfinding members
        int Fcost;              /**< Estimation of total path cost */
        int Gcost;              /**< Cost from start to this location */
        int Hcost;              /**< Estimated cost to goal */
        int whichList;          /**< No list, open list or closed list */
        int parentX;            /**< X coordinate of parent tile */
        int parentY;            /**< Y coordinate of parent tile */
        bool walkable;          /**< Can beings walk on this tile */
};

/**
 * A location on a tile map. Used for pathfinding, open list.
 */
class Location
{
    public:
        /**
         * Constructor.
         */
        Location(int x, int y, MetaTile *tile);

        /**
         * Comparison operator.
         */
        bool operator< (const Location &loc) const;

        int x, y;
        MetaTile *tile;
};

/**
 * A tile map.
 */
class Map
{
    public:
        /**
         * Constructor.
         */
        Map();

        /**
         * Constructor that takes initial map size as parameters.
         */
        Map(int width, int height);

        /**
         * Destructor.
         */
        ~Map();

        /**
         * Loads a map file (gat).
         */
        static Map *load(const std::string &mapFile);

        /**
         * Draws the map to the given graphics output.
         */
        void draw(Graphics *graphics, int scrollX, int scrollY, int layer);

        /**
         * Sets the size of the map. This will destroy any existing map data.
         */
        void setSize(int width, int height);

        /**
         * Set tile ID.
         */
        void setTile(int x, int y, int layer, Image *img);

        /**
         * Get tile ID.
         */
        Image *getTile(int x, int y, int layer);

        /**
         * Get tile reference.
         */
        MetaTile *getMetaTile(int x, int y);

        /**
         * Set walkability flag for a tile
         */
        void setWalk(int x, int y, bool walkable);

        /**
         * Tell if a tile is walkable or not, includes checking beings.
         */
        bool getWalk(int x, int y);

        /**
         * Tell if a tile collides, not including a check on beings.
         */
        bool tileCollides(int x, int y);

        /**
         * Returns the width of this map.
         */
        int getWidth();

        /**
         * Returns the height of this map.
         */
        int getHeight();

        /**
         * Returns the tile width of this map.
         */
        int getTileWidth();

        /**
         * Returns the tile height used by this map.
         */
        int getTileHeight();

        /**
         * Find a path from one location to the next.
         */
        std::list<PATH_NODE> findPath(
                int startX, int startY, int destX, int destY);

    private:
        int width, height;
        int tileWidth, tileHeight;
        MetaTile *metaTiles;
        Image **tiles;

        // Pathfinding members
        int onClosedList, onOpenList;
};

#endif
