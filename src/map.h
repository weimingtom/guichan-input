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

#ifndef _TMW_MAP_H_
#define _TMW_MAP_H_

#include <list>
#include <vector>

#include "properties.h"

class AmbientOverlay;
class Graphics;
class Image;
class Particle;
class Sprite;
class Tileset;

struct PATH_NODE;

typedef std::vector<Tileset*> Tilesets;
typedef std::list<Sprite*> Sprites;
typedef Sprites::iterator SpriteIterator;

extern volatile int tick_time;

/**
 * A meta tile stores additional information about a location on a tile map.
 * This is information that doesn't need to be repeated for each tile in each
 * layer of the map.
 */
struct MetaTile
{
    /**
     * Constructor.
     */
    MetaTile():whichList(0), blockmask(0) {};

    // Pathfinding members
    int Fcost;              /**< Estimation of total path cost */
    int Gcost;              /**< Cost from start to this location */
    int Hcost;              /**< Estimated cost to goal */
    int whichList;          /**< No list, open list or closed list */
    int parentX;            /**< X coordinate of parent tile */
    int parentY;            /**< Y coordinate of parent tile */
    unsigned char blockmask;          /**< Can beings walk on this tile */
};

/**
 * A tile map.
 */
class Map : public Properties
{
    public:
        enum BlockType
        {
            BLOCKTYPE_NONE = -1,
            BLOCKTYPE_WALL,
            BLOCKTYPE_CHARACTER,
            BLOCKTYPE_MONSTER,
            NB_BLOCKTYPES
        };

        /**
         * Constructor, taking map and tile size as parameters.
         */
        Map(int width, int height, int tileWidth, int tileHeight);

        /**
         * Destructor.
         */
        ~Map();

        /**
         * Initialize map overlays. Should be called after all the properties
         * are set.
         */
        void initializeOverlays();

        /**
         * Draws a map layer to the given graphics output.
         */
        void draw(Graphics *graphics, int scrollX, int scrollY, int layer);

        /**
         * Visualizes collision layer for debugging
         */
        void drawCollision(Graphics *graphics, int scrollX, int scrollY);

        /**
         * Draws the overlay graphic to the given graphics output.
         */
        void
        drawOverlay(Graphics *graphics, float scrollX, float scrollY,
                    int detail);

        /**
         * Adds a tileset to this map.
         */
        void
        addTileset(Tileset *tileset);

        /**
         * Sets a tile using a global tile id. Used by the layer loading
         * routine.
         */
        void
        setTileWithGid(int x, int y, int layer, int gid);

        /**
         * Set tile ID.
         */
        void setTile(int x, int y, int layer, Image *img);

        /**
         * Get tile ID.
         */
        Image *getTile(int x, int y, int layer) const;

        /**
         * Get tile reference.
         */
        MetaTile *getMetaTile(int x, int y) const;

        /**
         * Marks a tile as occupied
         */
        void blockTile(int x, int y, BlockType type);

        /**
         * Marks a tile as unoccupied
         */
        void freeTile(int x, int y, BlockType type);

        /**
         * Gets walkability for a tile with a blocking bitmask
         */
        bool getWalk(int x, int y, char walkmask) const;

        /**
         * Returns the width of this map.
         */
        int
        getWidth() const { return mWidth; }

        /**
         * Returns the height of this map.
         */
        int
        getHeight() const { return mHeight; }

        /**
         * Returns the tile width of this map.
         */
        int
        getTileWidth() const { return mTileWidth; }

        /**
         * Returns the tile height used by this map.
         */
        int
        getTileHeight() const { return mTileHeight; }

        /**
         * Find a path from one location to the next.
         */
        std::list<PATH_NODE>
        findPath(int startX, int startY, int destX, int destY, unsigned char walkmask, int maxCost = 20);

        /**
         * Adds a sprite to the map.
         */
        SpriteIterator
        addSprite(Sprite *sprite);

        /**
         * Removes a sprite from the map.
         */
        void
        removeSprite(SpriteIterator iterator);

        /**
         * Adds a particle effect
         */
        void addParticleEffect (std::string effectFile, int x, int y);

        /**
         * Initializes all added particle effects
         */
        void
        initializeParticleEffects(Particle* particleEngine);

    private:
        /**
         * Converts a global tile id to the Image* pointing to the associated
         * tile image.
         */
        Image* getTileWithGid(int gid) const;

        /**
         * Finds the tile set that a tile with the given global id is part of.
         */
        Tileset* getTilesetWithGid(int gid) const;

        /**
         * Tells whether the given coordinates fall within the map boundaries.
         */
        bool contains(int x, int y) const;

        /**
         * Blockmasks for different entities
         */
        static const unsigned char BLOCKMASK_WALL = 128;   // = bin 1000 0000
        static const unsigned char BLOCKMASK_CHARACTER = 1;// = bin 0000 0001
        static const unsigned char BLOCKMASK_MONSTER = 2;  // = bin 0000 0010
        int *mOccupation[NB_BLOCKTYPES];

        // Pathfinding members
        int onClosedList, onOpenList;

        int mWidth, mHeight;
        int mTileWidth, mTileHeight;
        int mMaxTileHeight;
        MetaTile *mMetaTiles;
        Image **mTiles;

        Tilesets mTilesets;
        Sprites mSprites;

        // Pathfinding members
        int mOnClosedList, mOnOpenList;

        // Overlay Data
        std::list<AmbientOverlay*> mOverlays;
        float mLastScrollX;
        float mLastScrollY;

        // Particle effect data
        struct ParticleEffectData
        {
            std::string file;
            int x;
            int y;
        };
        std::list<ParticleEffectData> particleEffects;
};

#endif
