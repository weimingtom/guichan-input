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
#include "beingmanager.h"
#include "configuration.h"
#include "guichanfwd.h"
#include "keyboardconfig.h"
#include "localplayer.h"
#include "log.h"
#include "main.h"
#include "npc.h"

#include "gui/chat.h"
#include "gui/npcdialog.h"
#include "gui/sdlinput.h"
#include "gui/setup_keyboard.h"

#include "utils/gettext.h"
#include "utils/stringutils.h"

struct KeyDefault
{
    const char *configField;
    int key;
    int mask;
    std::string caption;
};

enum
{
    KEY_META_RIGHT = -991,
    KEY_META_LEFT = -992,
    KEY_CTRL_RIGHT = -995,
    KEY_CTRL_LEFT = -996,
    KEY_SHIFT_RIGHT = -996,
    KEY_SHIFT_LEFT = -998,
    KEY_ALT_RIGHT = -999,
    KEY_ALT_LEFT = -1000
};

static KeyDefault const keyData[KeyboardConfig::KEY_TOTAL] = {
    {"keyMoveUp", Key::UP, 0 , _("Move Up")},
    {"keyMoveDown", Key::DOWN, 0, _("Move Down")},
    {"keyMoveLeft", Key::LEFT, 0, _("Move Left")},
    {"keyMoveRight", Key::RIGHT, 0, _("Move Right")},
    {"keyAttack", KEY_CTRL_LEFT, 0, _("Attack")},
    {"keyTalk", 't', 0, _("Talk")},
    {"keyTarget", KEY_SHIFT_LEFT, 0, _("Stop Attack")},
    {"keyTargetMonster", 'a', 0, _("Target MONSTER")},
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
    {"keyWindowParty", Key::F11, 0, _("Party Window")},
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
            // TODO: remove that
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

bool KeyboardConfig::keyMatch(int index, KeyData ev)
{
    return keyMatch(getKeyData(index), ev);
}

bool KeyboardConfig::keyMatch(KeyData data, KeyData ev)
{
    printf("(%d, %d) == (%d, %d)\n", data.key, data.mask, ev.key, ev.mask);
    return (ev.key == data.key || data.key == 0) && (ev.mask == data.mask);
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

    return ret;
}

static short key_state = 0;

enum {
    KS_UP = 0x1,
    KS_DOWN = 0x2,
    KS_LEFT = 0x4,
    KS_RIGHT = 0x8,
    KS_TARGET = 0x10,
    KS_TARGET_MONSTER = 0x20,
    KS_TARGET_PLAYER = 0x40,
    KS_TARGET_NPC = 0x80,
    KS_ATTACK = 0x100,
};

static Being::Type lastTarget = Being::UNKNOWN;

bool KeyboardConfig::isAttacking()
{
    return key_state & KS_ATTACK;
}

bool KeyboardConfig::isTargeting()
{
    return key_state & KS_TARGET;
}

void KeyboardConfig::keyPressed(gcn::KeyEvent &event)
{
    // Ignore consumed events
    if (event.isConsumed() || state != STATE_GAME)
        return;

    KeyData kd = keyConvert(event);

    if (parseMovement(kd, true))
        return;
    else if (keyMatch(KEY_TARGET, kd))
    {
        key_state |= KS_TARGET;
        player_node->stopAttack();
    }
    else if (parseTarget(kd, true))
        return;
    else if (keyMatch(KEY_ATTACK, kd))
    {
        key_state |= KS_ATTACK;
    }
    else if (keyMatch(KEY_TOGGLE_CHAT, kd))
    {
        chatWindow->requestChatFocus();
    }
    else if (keyMatch(KEY_SIT, kd))
    {
        player_node->toggleSit();
    }
    else if (keyMatch(KEY_TALK, kd))
    {
        if (!NPC::isTalking)
        {
            Being *target = player_node->getTarget();

            if (target && target->getType() == Being::NPC)
                dynamic_cast<NPC*>(target)->talk();
        }
    }
    // Otherwise, just print the key for now
    else
    {
        gcn::Key key = event.getKey();
        printf("%d %1$c\n", key.getValue());
    }
}

void KeyboardConfig::keyReleased(gcn::KeyEvent &event)
{
    // Ignore consumed events
    if (event.isConsumed() || state != STATE_GAME)
        return;

    KeyData kd = keyConvert(event);

    if (parseMovement(kd, false))
        return;
    else if (keyMatch(KEY_TARGET, kd))
    {
        key_state &= ~KS_TARGET;
    }
    else if (parseTarget(kd, false))
        return;
    else if (keyMatch(KEY_ATTACK, kd))
    {
        key_state &= ~KS_ATTACK;
    }
}

inline bool KeyboardConfig::parseMovement(KeyData kd, bool press)
{
    // Adjust the stored direction
    if (keyMatch(KEY_MOVE_UP, kd))
        if (press)
            key_state |= KS_UP;
        else
            key_state &= ~KS_UP;
    else if (keyMatch(KEY_MOVE_DOWN, kd))
        if (press)
            key_state |= KS_DOWN;
        else
            key_state &= ~KS_DOWN;
    else if (keyMatch(KEY_MOVE_LEFT, kd))
        if (press)
            key_state |= KS_LEFT;
        else
            key_state &= ~KS_LEFT;
    else if (keyMatch(KEY_MOVE_RIGHT, kd))
        if (press)
            key_state |= KS_RIGHT;
        else
            key_state &= ~KS_RIGHT;
    else return false; // If it's not a direction, we ignore it for now

    if (player_node->mAction == Being::DEAD && current_npc != 0)
        return true;

    // Now get the adjusted direction (remove conflicting directions)
    int direction = 0;

    if (key_state & KS_UP)
        direction |= Being::UP;
    else if (key_state & KS_DOWN)
        direction |= Being::DOWN;

    if (key_state & KS_LEFT)
        direction |= Being::LEFT;
    else if (key_state & KS_RIGHT)
        direction |= Being::RIGHT;

    // If the direction is different, then stop first (this makes things weird
    // for the eAthena build)
#ifdef TMWSERV_SUPPORT
    if (!real_direction || real_direction != player_node->getWalkingDir())
        player_node->stopWalking(false);
#endif

    // Set the player's direction
    player_node->setWalkingDir(direction);

#ifdef EATHENA_SUPPORT
    if (key_state & KS_ATTACK && lastTarget != Being::UNKNOWN)
    {
        Being *target = NULL;

        bool newTarget = !(key_state & KS_TARGET);
        // A set target has highest priority
        if (newTarget || !player_node->getTarget())
        {
            Uint16 targetX = player_node->mX, targetY = player_node->mY;

            switch (player_node->getSpriteDirection())
            {
                case DIRECTION_UP   : --targetY; break;
                case DIRECTION_DOWN : ++targetY; break;
                case DIRECTION_LEFT : --targetX; break;
                case DIRECTION_RIGHT: ++targetX; break;
                default: break;
            }

            // Only auto target Monsters
            target = beingManager->findNearestLivingBeing(targetX, targetY,
                     20, lastTarget);
        }

        player_node->attack(target, newTarget);
    }
#endif

    return true;
}

inline bool KeyboardConfig::parseTarget(KeyData kd, bool press)
{
    Being::Type currentTarget = Being::UNKNOWN;

    if (keyMatch(KEY_TARGET_MONSTER, kd))
        if (press)
            key_state |= KS_TARGET_MONSTER;
        else
            key_state &= ~KS_TARGET_MONSTER;
    else if (keyMatch(KEY_TARGET_NPC, kd))
        if (press)
            key_state |= KS_TARGET_NPC;
        else
            key_state &= ~KS_TARGET_NPC;
    else if (keyMatch(KEY_TARGET_PLAYER, kd))
        if (press)
            key_state |= KS_TARGET_PLAYER;
        else
            key_state &= ~KS_TARGET_PLAYER;
    else return false;

    if (key_state & KS_TARGET_MONSTER)
        currentTarget = Being::MONSTER;
    else if (key_state = KS_TARGET_NPC)
        currentTarget = Being::NPC;
    else if (key_state & KS_TARGET_PLAYER)
        currentTarget = Being::PLAYER;
    else lastTarget = Being::UNKNOWN; // Reset last target

    if (currentTarget == Being::UNKNOWN)
        return true;

    if (key_state & KS_TARGET) // We still want to consume the events,
        return true;             // but not process them

    Being *target = beingManager->findNearestLivingBeing(player_node, 20,
                                                         currentTarget);

    if (target && (target != player_node->getTarget() ||
            currentTarget != lastTarget))
    {
        player_node->setTarget(target);
        lastTarget = currentTarget;
    }
}
