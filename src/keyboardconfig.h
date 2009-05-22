/*
 *  Custom keyboard shortcuts configuration
 *  Copyright (C) 2007  Joshua Langley <joshlangley@optusnet.com.au>
 *
 *  This file is part of The Mana World.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef KEYBOARDCONFIG_H
#define KEYBOARDCONFIG_H


#include <guichan/keylistener.hpp>

#include <SDL_types.h>

#include <map>
#include <string>

enum {
    KEY_MASK_SHIFT = 1,
    KEY_MASK_CTRL = 2,
    KEY_MASK_ALT = 4,
    KEY_MASK_META = 8
};

/**
 * Each key represents the basic data needed for a key function.
 */
struct KeyData
{
    int key;    /** The actual guichan keycode that is used. */
    char mask;   /** The modifiers to be used. */
};

class Setup_Keyboard;

class KeyboardConfig : public gcn::KeyListener
{
    public:
        KeyboardConfig();

        /**
         * Initializes the keyboard config explicitly.
         */
        void init();

        /**
         * Retrieve the key values from config file.
         */
        void retrieve();

        /**
         * Store the key values to config file.
         */
        void store();

        /**
         * Resets all the keys to their default values.
         */
        void resetToDefaults();

        /**
         * Determines if any key assignments are the same as each other.
         */
        bool hasConflicts();

        /**
         * Returns the key data for the given key.
         */
        KeyData getKeyData(int index);

        /**
         * Changes the given key to match the given key data.
         */
        void setKeyData(int index, KeyData data);

        /**
         * Get the enable flag, which will stop the user from doing actions.
         */
        bool isEnabled() const
        { return mEnabled; }

        /**
         * Get the key's caption, providing more meaning to the user.
         */
        const std::string &getKeyCaption(int index) const;

        /**
         * Get the key function index by providing the keys value.
         */
        int getKeyIndex(int keyValue) const;

        /**
         * Set the enable flag, which will stop the user from doing actions.
         */
        void setEnabled(bool flag)
        { mEnabled = flag; }

        /**
         * Set a reference to the key setup window.
         */
        void setSetupKeyboard(Setup_Keyboard *setupKey)
        { mSetupKey = setupKey; }

        /**
         * Returns whether the given action and event match. This is a
         * convinience for the KeyData form.
         */
        bool keyMatch(int index, KeyData ret);

        /**
         * Returns whether the given action and key data match.
         */
        bool keyMatch(KeyData data, KeyData ret);

        /**
         * Returns a string represnetation of the given key data.
         */
        std::string keyString(KeyData data);

        /**
         * Returns a string represnetation of the given key data.
         */
        std::string keyString(int index)
        { return keyString(getKeyData(index));  }

        /**
         * Tries to parse the given string as a key data.
         */
        KeyData keyParse(std::string keyString);

        /**
         * Turns the given event into key data.
         */
        KeyData keyConvert(gcn::KeyEvent &event);

        void resetStates();

        void processStates();

        bool isKeyActive(int key);

        void keyPressed(gcn::KeyEvent &event);

        void keyReleased(gcn::KeyEvent &event);

        /**
         * All the key functions.
         * KEY_NO_VALUE is used in initialization, and should be unchanged.
         * KEY_TOTAL should always be last (used as a conditional in loops).
         * The key assignment view gets arranged according to the order of
         * these values.
         */
        enum KeyAction
        {
            KEY_NO_VALUE = -1,
            KEY_MOVE_UP,
            KEY_MOVE_DOWN,
            KEY_MOVE_LEFT,
            KEY_MOVE_RIGHT,
            KEY_ATTACK,
            KEY_TALK,
            KEY_TARGET_ATTACK,
            KEY_TARGET_MONSTER,
            KEY_TARGET_NPC,
            KEY_TARGET_PLAYER,
            KEY_PICKUP,
            KEY_HIDE_WINDOWS,
            KEY_SIT,
            KEY_SCREENSHOT,
            KEY_TRADE,
            KEY_PATHFIND,
            KEY_SHORTCUT_1,
            KEY_SHORTCUT_2,
            KEY_SHORTCUT_3,
            KEY_SHORTCUT_4,
            KEY_SHORTCUT_5,
            KEY_SHORTCUT_6,
            KEY_SHORTCUT_7,
            KEY_SHORTCUT_8,
            KEY_SHORTCUT_9,
            KEY_SHORTCUT_10,
            KEY_SHORTCUT_11,
            KEY_SHORTCUT_12,
            KEY_WINDOW_HELP,
            KEY_WINDOW_STATUS,
            KEY_WINDOW_INVENTORY,
            KEY_WINDOW_EQUIPMENT,
            KEY_WINDOW_SKILL,
            KEY_WINDOW_MINIMAP,
            KEY_WINDOW_CHAT,
            KEY_WINDOW_SHORTCUT,
            KEY_WINDOW_SETUP,
            KEY_WINDOW_DEBUG,
            KEY_WINDOW_PARTY,
            KEY_WINDOW_EMOTE_SHORTCUT,
            KEY_WINDOW_OUTFIT,
            KEY_OUTFIT_1,
            KEY_OUTFIT_2,
            KEY_OUTFIT_3,
            KEY_OUTFIT_4,
            KEY_OUTFIT_5,
            KEY_OUTFIT_6,
            KEY_OUTFIT_7,
            KEY_OUTFIT_8,
            KEY_OUTFIT_9,
            KEY_OUTFIT_10,
            KEY_EMOTE_1,
            KEY_EMOTE_2,
            KEY_EMOTE_3,
            KEY_EMOTE_4,
            KEY_EMOTE_5,
            KEY_EMOTE_6,
            KEY_EMOTE_7,
            KEY_EMOTE_8,
            KEY_EMOTE_9,
            KEY_EMOTE_10,
            KEY_EMOTE_11,
            KEY_EMOTE_12,
            KEY_TOGGLE_CHAT,
            KEY_SCROLL_CHAT_UP,
            KEY_SCROLL_CHAT_DOWN,
            KEY_PREV_CHAT_TAB,
            KEY_NEXT_CHAT_TAB,
            KEY_OK,
            KEY_QUIT,
            KEY_TOTAL
        };

        static KeyData NULL_KEY;

    private:
        int enumValue(KeyData key);

        KeyboardConfig::KeyAction mNewKeyIndex; /**< Index of new key to be assigned */
        bool mEnabled;                 /**< Flag to respond to key input */

        Setup_Keyboard *mSetupKey;     /**< Reference to setup window */

        typedef std::map<int, std::string> KeyDescMap;
        KeyDescMap mDescs;

        bool mStates[KEY_TOTAL];

        KeyData mKey[KEY_TOTAL];       /**< Pointer to all the key data */

        Uint8 *mActiveKeys;            /**< Stores a list of all the keys */

        inline void parseMovement();
        inline void parseTarget();
        inline void parseWindows();
        inline void parseItemShortcut();
        inline void parseEmoteShortcut();
        inline void parseOutfitShortcut();
};

extern KeyboardConfig keyboard;

#endif
