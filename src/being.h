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

#ifndef _TMW_BEING_H
#define _TMW_BEING_H

#include <list>
#include <string>
#include <SDL_types.h>
#include <vector>

#include "position.h"
#include "sprite.h"
#include "map.h"
#include "animatedsprite.h"
#include "vector.h"

#define NR_HAIR_STYLES 8
#define NR_HAIR_COLORS 10

class AnimatedSprite;
class Equipment;
class ItemInfo;
class Item;
class Map;
class Graphics;
class ImageSet;
class Particle;
class SpeechBubble;

class Being : public Sprite
{
    public:
        enum Type {
            UNKNOWN,
            PLAYER,
            NPC,
            MONSTER
        };

        /**
         * Action the being is currently performing
         * WARNING: Has to be in sync with the same enum in the Being class
         * of the server!
         */
        enum Action {
            STAND,
            WALK,
            ATTACK,
            SIT,
            DEAD,
            HURT
        };

        enum Sprite {
            BASE_SPRITE = 0,
            SHOE_SPRITE,
            BOTTOMCLOTHES_SPRITE,
            TOPCLOTHES_SPRITE,
            HAIR_SPRITE,
            HAT_SPRITE,
            WEAPON_SPRITE,
            VECTOREND_SPRITE
        };

        enum TargetCursorSize {
            TC_SMALL = 0,
            TC_MEDIUM,
            TC_LARGE,
            NUM_TC
        };


        /**
         * Directions, to be used as bitmask values
         */
        enum { DOWN = 1, LEFT = 2, UP = 4, RIGHT = 8 };

        std::string mName;      /**< Name of character */
        Uint8 mEmotion;         /**< Currently showing emotion */
        Uint8 mEmotionTime;     /**< Time until emotion disappears */
        Uint16 mAttackSpeed;    /**< Attack speed */
        Action mAction;         /**< Action the being is performing */
        Uint16 mJob;            /**< Job (player job, npc, monster, creature ) */

        /**
         * Constructor.
         */
        Being(int id, int job, Map *map);

        /**
         * Destructor.
         */
        virtual ~Being();

        /**
         * Removes all path nodes from this being.
         */
        void clearPath();

        /**
         * Sets a new destination for this being to walk to.
         */
        void setDestination(int x, int y);

        /**
         * Returns the destination for this being.
         */
        const Vector &getDestination() const { return mDest; }

        /**
         * Adjusts course to expected stat point.
         */
        void adjustCourse(int, int);

        /**
         * Adjusts course to expected start and end points.
         */
        void adjustCourse(int, int, int, int);

        /**
         * Puts a "speech balloon" above this being for the specified amount
         * of time.
         *
         * @param text The text that should appear.
         * @param time The amount of time the text should stay in milliseconds.
         */
        void setSpeech(const std::string &text, Uint32 time);

        /**
         * Puts a damage bubble above this being.
         *
         * @param amount The amount of damage.
         */
        virtual void
        takeDamage(int amount);

        /**
         * Handles an attack of another being by this being.
         */
        virtual void
        handleAttack();

        /**
         * Returns the name of the being.
         */
        const std::string&
        getName() const { return mName; }

        /**
         * Sets the name for the being.
         *
         * @param name The name that should appear.
         */
        void
        setName(const std::string &name) { mName = name; }

        /**
         * Sets visible equipments for this being.
         */
        virtual void
        setSprite(int slot, int id, const std::string &color = "");

        /**
         * Performs being logic.
         */
        virtual void
        logic();

        /**
         * Draws the speech text above the being.
         */
        void
        drawSpeech(Graphics* graphics, int offsetX, int offsetY);

        /**
         * Draws the emotion picture above the being.
         */
        void
        drawEmotion(Graphics *graphics, int offsetX, int offsetY);

        /**
         * Draws the name text below the being.
         */
        virtual void
        drawName(Graphics *, int, int) {};

        /**
         * Returns the type of the being.
         */
        virtual Type getType() const;

        /**
         * Gets the walk speed.
         * @see setWalkSpeed(int)
         */
        int getWalkSpeed() const { return mWalkSpeed; }

        /**
         * Sets the walk speed (in pixels per second).
         */
        void setWalkSpeed(int speed) { mWalkSpeed = speed; }

        /**
         * Gets the being id.
         */
        Uint16
        getId() const { return mId; }

        /**
         * Sets the sprite id.
         */
        void
        setId(Uint16 id) { mId = id; }

        /**
         * Sets the map the being is on
         */
        void setMap(Map *map);

        /**
         * Sets the current action.
         */
        virtual void
        setAction(Action action, int attackType = 0);

        /**
         * Gets the current action.
         */
        bool isAlive() { return mAction != DEAD; }

        /**
         * Returns the direction the being is facing.
         */
        SpriteDirection getSpriteDirection() const
        { return SpriteDirection(mSpriteDirection); }

        /**
         * Sets the current direction.
         */
        void setDirection(Uint8 direction);

        /**
         * Draws this being to the given graphics context.
         *
         * @see Sprite::draw(Graphics, int, int)
         */
        virtual void draw(Graphics *graphics, int offsetX, int offsetY) const;

        /**
         * Returns the pixel X coordinate.
         */
        int getPixelX() const { return (int) mPos.x; }

        /**
         * Returns the pixel Y coordinate.
         *
         * @see Sprite::getPixelY()
         */
        int getPixelY() const { return (int) mPos.y; }

        /**
         * Sets the position of this being.
         */
        void setPosition(const Vector &pos);

        /**
         * Overloaded method provided for convenience.
         *
         * @see setPosition(const Vector &pos)
         */
        void setPosition(float x, float y, float z = 0.0f)
        {
            setPosition(Vector(x, y, z));
        }

        /**
         * Returns the position of this being.
         */
        const Vector &getPosition() const { return mPos; }

        /**
         * Returns the horizontal size of the current base sprite of the being.
         */
        virtual int getWidth() const;

        /**
         * Returns the vertical size of the current base sprite of the being.
         */
        virtual int getHeight() const;

        /**
         * Returns the required size of a target cursor for this being.
         */
        virtual Being::TargetCursorSize getTargetCursorSize() const
        { return TC_MEDIUM; }

        /**
         * Take control of a particle.
         */
        void controlParticle(Particle *particle);

        /**
         * Gets the way the object is blocked by other objects.
         */
        virtual unsigned char getWalkMask() const
        { return 0x00; } //can walk through everything

        /**
         * Returns the path this being is following. An empty path is returned
         * when this being isn't following any path currently.
         */
        const Path &getPath() const { return mPath; }

    protected:
        /**
         * Sets the new path for this being.
         */
        void setPath(const Path &path);

        /**
         * Gets the way the object blocks pathfinding for other objects
         */
        virtual Map::BlockType getBlockType() const
        { return Map::BLOCKTYPE_NONE; }

        Uint16 mId;                     /**< Unique being id */
        Uint8 mSpriteDirection;         /**< Facing direction */
        Uint8 mDirection;               /**< Walking direction */
        Map *mMap;                      /**< Map on which this being resides */
        SpriteIterator mSpriteIterator;

        /** Engine-related infos about weapon. */
        const ItemInfo* mEquippedWeapon;

        Path mPath;
        std::string mSpeech;
        Uint32 mSpeechTime;

        std::vector<AnimatedSprite*> mSprites;
        std::vector<int> mSpriteIDs;
        std::vector<std::string> mSpriteColors;
        std::list<Particle *> mChildParticleEffects;

    private:
        // Speech Bubble components
        SpeechBubble *mSpeechBubble;

        int mWalkSpeed;                 /**< Walking speed (pixels/sec) */

        Vector mPos;
        Vector mDest;

        static int instances;           /**< Number of Being instances */
        static ImageSet *emotionSet;    /**< Emoticons used by beings */
};

#endif
