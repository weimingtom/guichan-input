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

#ifndef _TMW_VIEWPORT_H_
#define _TMW_VIEWPORT_H_

#include <guichan/mouselistener.hpp>

#include "windowcontainer.h"

#include "../configlistener.h"
#include "../being.h"

class Map;
class FloorItem;
class ImageSet;
class Item;
class PopupMenu;
class Graphics;
class SimpleAnimation;

/** Delay between two mouse calls when dragging mouse and move the player */
const int walkingMouseDelay = 500;

/**
 * The viewport on the map. Displays the current map and handles mouse input
 * and the popup menu.
 *
 * TODO: This class is planned to be extended to allow floating widgets on top
 * of it such as NPC messages, which are positioned using map pixel
 * coordinates.
 */
class Viewport : public WindowContainer, public gcn::MouseListener,
    public ConfigListener
{
    public:
        /**
         * Constructor.
         */
        Viewport();

        /**
         * Destructor.
         */
        ~Viewport();

        /**
         * Sets the map displayed by the viewport.
         */
        void
        setMap(Map *map);

        /**
         * Draws the viewport.
         */
        void
        draw(gcn::Graphics *graphics);

        /**
         * Implements player to keep following mouse.
         */
        void
        logic();

        /**
         * Toggles whether the path debug graphics are shown
         */
        void
        toggleDebugPath() { mShowDebugPath = !mShowDebugPath; }

        /**
         * Handles mouse press on map.
         */
        void
        mousePressed(gcn::MouseEvent &event);

        /**
         * Handles mouse move on map
         */
        void
        mouseDragged(gcn::MouseEvent &event);

        /**
         * Handles mouse button release on map.
         */
        void
        mouseReleased(gcn::MouseEvent &event);

        /**
         * Handles mouse move on map.
         */
        void
        mouseMoved(gcn::MouseEvent &event);

        /**
         * Shows a popup for an item.
         * TODO Find some way to get rid of Item here
         */
        void
        showPopup(int x, int y, Item *item);

        /**
         * A relevant config option changed.
         */
        void
        optionChanged(const std::string &name);

        /**
         * Returns camera x offset in pixels.
         */
        int
        getCameraX() const { return (int) mViewX; }

        /**
         * Returns camera y offset in pixels.
         */
        int
        getCameraY() const { return (int) mViewY; }

        /**
         * Changes viewpoint by relative pixel coordinates.
         */
        void
        scrollBy(float x, float y) { mViewX += x; mViewY += y; }

    private:
        /**
         * Helper function for loading target cursors.
         */
        void loadTargetCursor(const std::string &filename,
                              int width, int height,
                              bool outRange, Being::TargetCursorSize size);

        /**
         * Draws range based target cursor
         */
        void drawTargetCursor(Graphics *graphics);

        /**
         * Draws target name
         */
        void drawTargetName(Graphics *graphics);

        /**
         * Finds a path from the player to the mouse, and draws it. This is for
         * debug purposes.
         */
        void drawDebugPath(Graphics *graphics);

        /**
         * Draws the given path.
         */
        void drawPath(Graphics *graphics, const Path &path);


        Map *mMap;                 /**< The current map. */

        int mScrollRadius;
        int mScrollLaziness;
        int mScrollCenterOffsetX;
        int mScrollCenterOffsetY;
        float mViewX;              /**< Current viewpoint in pixels. */
        float mViewY;              /**< Current viewpoint in pixels. */
        bool mShowDebugPath;       /**< Show a path from player to pointer. */
        bool mVisibleNames;        /**< Show target names. */

        /** Images of in range target cursor. */
        ImageSet *mInRangeImages[Being::NUM_TC];

        /** Images of out of range target cursor. */
        ImageSet *mOutRangeImages[Being::NUM_TC];

        /** Animated in range target cursor. */
        SimpleAnimation *mTargetCursorInRange[Being::NUM_TC];

        /** Animated out of range target cursor. */
        SimpleAnimation *mTargetCursorOutRange[Being::NUM_TC];

        bool mPlayerFollowMouse;
        int mLocalWalkTime; /**< Timestamp before the next walk can be sent. */

        PopupMenu *mPopupMenu;     /**< Popup menu. */
        Being *mSelectedBeing;     /**< Current selected being. */
};

#endif
