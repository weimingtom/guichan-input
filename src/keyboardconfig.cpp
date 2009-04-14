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

#include "being.h"
#include "configuration.h"
#include "guichanfwd.h"
#include "keyboardconfig.h"
#include "localplayer.h"
#include "log.h"

#include "gui/chat.h"
#include "gui/sdlinput.h"
#include "gui/setup_keyboard.h"

#include "utils/gettext.h"
#include "utils/stringutils.h"
#include "utils/strprintf.h"

struct KeyDefault
{
    const char *configField;
    int key;
    int mask;
    std::string caption;
};

static KeyDefault const keyData[KeyboardConfig::KEY_TOTAL] = {
    {"keyMoveUp", Key::UP, 0 , _("Move Up")},
    {"keyMoveDown", Key::DOWN, 0, _("Move Down")},
    {"keyMoveLeft", Key::LEFT, 0, _("Move Left")},
    {"keyMoveRight", Key::RIGHT, 0, _("Move Right")},
    {"keyAttack", 0, KEY_MASK_CTRL, _("Attack")},
    {"keyTalk", 't', 0, _("Talk")},
    {"keyTarget", 0, KEY_MASK_SHIFT, _("Stop Attack")},
    {"keyTargetClosest", 'a', 0, _("Target Closest")},
    {"keyTargetNPC", 'n', 0, _("Target NPC")},
    {"keyTargetPlayer", 'q', 0, _("Target Player")},
    {"keyPickup", 'z', 0, _("Pickup")},
    {"keyHideWindows", 'h', 0, _("Hide Windows")},
    {"keyBeingSit", 's', 0, _("Sit")},
    {"keyScreenshot", 'p', 0, _("Screenshot")},
    {"keyTrade", 'r', 0, _("Enable/Disable Trading")},
    {"keyPathfind", 'f', 0, _("Find Path to Mouse")},
    {"keyShortcut1", '1', 0, strprintf(_("Item Shortcut %d"), 1)},
    {"keyShortcut2", '2', 0, strprintf(_("Item Shortcut %d"), 2)},
    {"keyShortcut3", '3', 0, strprintf(_("Item Shortcut %d"), 3)},
    {"keyShortcut4", '4', 0, strprintf(_("Item Shortcut %d"), 4)},
    {"keyShortcut5", '5', 0, strprintf(_("Item Shortcut %d"), 5)},
    {"keyShortcut6", '6', 0, strprintf(_("Item Shortcut %d"), 6)},
    {"keyShortcut7", '7', 0, strprintf(_("Item Shortcut %d"), 7)},
    {"keyShortcut8", '8', 0, strprintf(_("Item Shortcut %d"), 8)},
    {"keyShortcut9", '9', 0, strprintf(_("Item Shortcut %d"), 9)},
    {"keyShortcut10", '0', 0, strprintf(_("Item Shortcut %d"), 10)},
    {"keyShortcut11", '-', 0, strprintf(_("Item Shortcut %d"), 11)},
    {"keyShortcut12", '=', 0, strprintf(_("Item Shortcut %d"), 12)},
    {"keyWindowHelp", Key::F1, 0, _("Help Window")},
    {"keyWindowStatus", Key::F2, 0, _("Status Window")},
    {"keyWindowInventory", Key::F3, 0, _("Inventory Window")},
    {"keyWindowEquipment", Key::F4, 0, _("Equipment WIndow")},
    {"keyWindowSkill", Key::F5, 0, _("Skill Window")},
    {"keyWindowMinimap", Key::F6, 0, _("Minimap Window")},
    {"keyWindowChat", Key::F7, 0, _("Chat Window")},
    {"keyWindowShortcut", Key::F8, 0, _("Item Shortcut Window")},
    {"keyWindowSetup", Key::F9, 0, _("Setup Window")},
    {"keyWindowDebug", Key::F10, 0, _("Debug Window")},
    {"keyWindowEmote", Key::F11, 0, _("Emote Window")},
    {"keyWindowEmoteBar", Key::F12, 0, _("Emote Shortcut Window")},
    {"keyEmoteShortcut1", '1', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 1)},
    {"keyEmoteShortcut2", '2', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 2)},
    {"keyEmoteShortcut3", '3', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 3)},
    {"keyEmoteShortcut4", '4', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 4)},
    {"keyEmoteShortcut5", '5', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 5)},
    {"keyEmoteShortcut6", '6', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 6)},
    {"keyEmoteShortcut7", '7', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 7)},
    {"keyEmoteShortcut8", '8', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 8)},
    {"keyEmoteShortcut9", '9', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 9)},
    {"keyEmoteShortcut10", '0', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 10)},
    {"keyEmoteShortcut11", '-', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 11)},
    {"keyEmoteShortcut12", '=', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 12)},
    {"keyChat", Key::ENTER, 0, _("Toggle Chat")},
    {"keyChatScrollUp", Key::PAGE_UP, 0, _("Scroll Chat Up")},
    {"keyChatScrollDown", Key::PAGE_DOWN, 0, _("Scroll Chat Down")},
    {"keyChatPrevTab", '[', 0, _("Previous Chat Tab")},
    {"keyChatNextTab", ']', 0, _("Next Chat Tab")},
    {"keyOK", Key::ENTER, 0, _("Select OK")},
    {"keyQuit", Key::ESCAPE, 0, _("Quit")},
};

KeyboardConfig::KeyboardConfig()
{
    mDescs[0] = "None";
    mDescs[Key::BACKSPACE] = "Backspace";
    mDescs[Key::CAPS_LOCK] = "CapsLock";
    mDescs[Key::DELETE] = "Delete";
    mDescs[Key::DOWN] = "Down";
    mDescs[Key::END] = "End";
    mDescs[Key::ENTER] = "Enter";
    mDescs[Key::ESCAPE] = "Escape";
    mDescs[Key::F1] = "F1";
    mDescs[Key::F2] = "F2";
    mDescs[Key::F3] = "F3";
    mDescs[Key::F4] = "F4";
    mDescs[Key::F5] = "F5";
    mDescs[Key::F6] = "F6";
    mDescs[Key::F7] = "F7";
    mDescs[Key::F8] = "F8";
    mDescs[Key::F9] = "F9";
    mDescs[Key::F10] = "F10";
    mDescs[Key::F11] = "F11";
    mDescs[Key::F12] = "F12";
    mDescs[Key::F13] = "F13";
    mDescs[Key::F14] = "F14";
    mDescs[Key::F15] = "F15";
    mDescs[Key::HOME] = "Home";
    mDescs[Key::INSERT] = "Insert";
    mDescs[Key::LEFT] = "Left";
    mDescs[Key::NUM_LOCK] = "NumLock";
    mDescs[Key::PAGE_DOWN] = "PageDown";
    mDescs[Key::PAGE_UP] = "PageUp";
    mDescs[Key::PAUSE] = "Pause";
    mDescs[Key::PRINT_SCREEN] = "PrintScreen";
    mDescs[Key::RIGHT] = "Right";
    mDescs[Key::SCROLL_LOCK] = "ScrollLock";
    mDescs[Key::SPACE] = "Space";
    mDescs[Key::TAB] = "Tab";
    mDescs[Key::UP] = "Up";
}

void KeyboardConfig::init()
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        mKey[i].key = keyData[i].key;
        mKey[i].mask = keyData[i].mask;
    }
    mNewKeyIndex = KEY_NO_VALUE;
    mEnabled = true;

    retrieve();
}

void KeyboardConfig::retrieve()
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        //mKey[i] = keyParse(config.getValue(keyData[i].configField));
        KeyData kd = {keyData[i].key, keyData[i].mask};
        mKey[i] = kd;
        //std::string str = keyString(kd);
        //printf("%s\n", str.c_str());
    }
}

void KeyboardConfig::store()
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        config.setValue(keyData[i].configField, keyString(mKey[i]));
    }
}

void KeyboardConfig::resetToDefaults()
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        mKey[i].key = mKey[i].key;
        mKey[i].mask = mKey[i].mask;
    }
}

bool KeyboardConfig::hasConflicts()
{
    int i, j;
    /**
     * No need to parse the square matrix: only check one triangle
     * that's enough to detect conflicts
     */
    for (i = 0; i < KEY_TOTAL; i++)
    {
        for (j = i, j++; j < KEY_TOTAL; j++)
        {
            // Allow for item shortcut and emote keys to overlap, but no other keys
            if (!((((i >= KEY_SHORTCUT_1) && (i <= KEY_SHORTCUT_12)) &&
                   ((j >= KEY_EMOTE_1) && (j <= KEY_EMOTE_12))) ||
                   ((i == KEY_TOGGLE_CHAT) && (j == KEY_OK))) &&
                   (mKey[i].key == mKey[j].key)
               )
            {
                return true;
            }
        }
    }
    return false;
}

void KeyboardConfig::callbackNewKey()
{
    mSetupKey->newKeyCallback(mNewKeyIndex);
}

int KeyboardConfig::getKeyIndex(int keyValue) const
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        if (keyValue == mKey[i].key)
        {
            return i;
        }
    }
    return KEY_NO_VALUE;
}


int KeyboardConfig::getKeyEmoteOffset(int keyValue) const
{
    for (int i = KEY_EMOTE_1; i <= KEY_EMOTE_12; i++)
    {
        if (keyValue == mKey[i].key)
        {
            return 1 + i - KEY_EMOTE_1;
        }
    }
    return 0;
}

KeyData KeyboardConfig::getKeyData(int index)
{
    return mKey[index];
}

void KeyboardConfig::setKeyData(int index, KeyData data)
{
    mKey[index] = data;
}

const std::string &KeyboardConfig::getKeyCaption(int index) const
{
    return keyData[index].caption;
}

bool KeyboardConfig::keyMatch(int index, gcn::KeyEvent &event)
{
    return keyMatch(getKeyData(index), event);
}

bool KeyboardConfig::keyMatch(KeyData data, gcn::KeyEvent &event)
{
    KeyData ev = keyConvert(event);

    return (ev.key == data.key) && (ev.mask == data.mask);
}

std::string KeyboardConfig::keyString(KeyData data)
{
    std::stringstream out;

    if (data.mask & KEY_MASK_SHIFT)
        out << "Shift+";

    if (data.mask & KEY_MASK_CTRL)
        out << "Ctrl+";

    if (data.mask & KEY_MASK_ALT)
        out << "Alt+";

    if (data.mask & KEY_MASK_META)
        out << "Meta+";

    // If it's printable, then do so
    if (data.key > 32 && data.key < 256)
        out << (char) data.key;
    // Otherwise, check our map
    else
    {
        KeyDescMap::iterator it = mDescs.find(data.key);
        if (it != mDescs.end())
            out << it->second;
        // Anything else, just quote the numeric value
        else
            out << "'" + toString(data.key) + "'";
    }

    return out.str();
}

KeyData KeyboardConfig::keyParse(std::string *keyString)
{
    // TODO
    KeyData ret = {0, 0};
    return ret;
}

KeyData KeyboardConfig::keyConvert(gcn::KeyEvent &event)
{
    KeyData ret;
    ret.key = event.getKey().getValue();
    ret.mask = 0;

    if (event.isShiftPressed())
        ret.mask |= KEY_MASK_SHIFT;

    if (event.isControlPressed())
        ret.mask |= KEY_MASK_CTRL;

    if (event.isAltPressed())
        ret.mask |= KEY_MASK_ALT;

    if (event.isMetaPressed())
        ret.mask |= KEY_MASK_META;

    if (ret.key < 0)
        ret.key = 0;

    return ret;
}

void KeyboardConfig::keyPressed(gcn::KeyEvent &event)
{
    // Ignore consumed events
    if (event.isConsumed())
        return;

    gcn::Key key = event.getKey();

    // This will open the chat input box on enter
    if (keyMatch(KEY_TOGGLE_CHAT, event))
    {
        chatWindow->requestChatFocus();
    }
    // Otherwise, just print the key for now
    else
        printf("%d %1$c\n", key.getValue());
}

void KeyboardConfig::keyReleased(gcn::KeyEvent &event)
{
    // Ignore consumed events
    if (event.isConsumed())
        return;

    // TODO
}
