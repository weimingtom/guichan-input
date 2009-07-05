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
#include "emoteshortcut.h"
#include "game.h"
#include "guichanfwd.h"
#include "itemshortcut.h"
#include "keyboardconfig.h"
#include "localplayer.h"
#include "log.h"
#include "main.h"
#include "npc.h"
#include "playerrelations.h"

#include "gui/widgets/chattab.h"
#include "gui/chat.h"
#include "gui/debugwindow.h"
#include "gui/equipmentwindow.h"
#include "gui/gui.h"
#include "gui/help.h"
#include "gui/inventorywindow.h"
#include "gui/minimap.h"
#include "gui/npcdialog.h"
#include "gui/outfitwindow.h"
#include "gui/partywindow.h"
#include "gui/sdlinput.h"
#include "gui/setup.h"
#include "gui/setup_keyboard.h"
#include "gui/shortcutwindow.h"
#include "gui/skill.h"
#include "gui/status.h"
#include "gui/viewport.h"

#include "utils/gettext.h"
#include "utils/stringutils.h"

KeyData KeyboardConfig::NULL_KEY = {0, 0};

struct KeyDefault
{
    const char *configField;
    int key;
    int mask;
    std::string caption;
};

static KeyDefault const keyData[KeyboardConfig::KEY_TOTAL] = {
    {"key_MoveUp", Key::UP, 0 , _("Move Up")},
    {"key_MoveDown", Key::DOWN, 0, _("Move Down")},
    {"key_MoveLeft", Key::LEFT, 0, _("Move Left")},
    {"key_MoveRight", Key::RIGHT, 0, _("Move Right")},
    {"key_Attack", Key::LEFT_CONTROL, 0, _("Attack")},
    {"key_Talk", 't', 0, _("Talk")},
    {"key_Target", Key::LEFT_SHIFT, 0, _("Target & Attack")},
    {"key_TargetMonster", 'a', 0, _("Target MONSTER")},
    {"key_TargetNPC", 'n', 0, _("Target NPC")},
    {"key_TargetPlayer", 'q', 0, _("Target Player")},
    {"key_Pickup", 'z', 0, _("Pickup")},
    {"key_HideWindows", 'h', 0, _("Hide Windows")},
    {"key_BeingSit", 's', 0, _("Sit")},
    {"key_Screenshot", 'p', 0, _("Screenshot")},
    {"key_Trade", 'r', 0, _("Enable/Disable Trading")},
    {"key_Pathfind", 'f', 0, _("Find Path to Mouse")},
    {"key_Shortcut1", '1', 0, strprintf(_("Item Shortcut %d"), 1)},
    {"key_Shortcut2", '2', 0, strprintf(_("Item Shortcut %d"), 2)},
    {"key_Shortcut3", '3', 0, strprintf(_("Item Shortcut %d"), 3)},
    {"key_Shortcut4", '4', 0, strprintf(_("Item Shortcut %d"), 4)},
    {"key_Shortcut5", '5', 0, strprintf(_("Item Shortcut %d"), 5)},
    {"key_Shortcut6", '6', 0, strprintf(_("Item Shortcut %d"), 6)},
    {"key_Shortcut7", '7', 0, strprintf(_("Item Shortcut %d"), 7)},
    {"key_Shortcut8", '8', 0, strprintf(_("Item Shortcut %d"), 8)},
    {"key_Shortcut9", '9', 0, strprintf(_("Item Shortcut %d"), 9)},
    {"key_Shortcut10", '0', 0, strprintf(_("Item Shortcut %d"), 10)},
    {"key_Shortcut11", '-', 0, strprintf(_("Item Shortcut %d"), 11)},
    {"key_Shortcut12", '=', 0, strprintf(_("Item Shortcut %d"), 12)},
    {"key_WindowHelp", Key::F1, 0, _("Help Window")},
    {"key_WindowStatus", Key::F2, 0, _("Status Window")},
    {"key_WindowInventory", Key::F3, 0, _("Inventory Window")},
    {"key_WindowEquipment", Key::F4, 0, _("Equipment Window")},
    {"key_WindowSkill", Key::F5, 0, _("Skill Window")},
    {"key_WindowMinimap", Key::F6, 0, _("Minimap Window")},
    {"key_WindowChat", Key::F7, 0, _("Chat Window")},
    {"key_WindowShortcut", Key::F8, 0, _("Item Shortcut Window")},
    {"key_WindowSetup", Key::F9, 0, _("Setup Window")},
    {"key_WindowDebug", Key::F10, 0, _("Debug Window")},
    {"key_WindowParty", Key::F11, 0, _("Party Window")},
    {"key_WindowEmoteBar", Key::F12, 0, _("Emote Shortcut Window")},
    {"key_WindowOutfit", 'o', 0, _("Outfit Shortcut Window")},
    {"key_OutfitShortcut1", '1', KEY_MASK_CTRL, _("Outfit Shortcut 1")},
    {"key_OutfitShortcut2", '2', KEY_MASK_CTRL, _("Outfit Shortcut 2")},
    {"key_OutfitShortcut3", '3', KEY_MASK_CTRL, _("Outfit Shortcut 3")},
    {"key_OutfitShortcut4", '4', KEY_MASK_CTRL, _("Outfit Shortcut 4")},
    {"key_OutfitShortcut5", '5', KEY_MASK_CTRL, _("Outfit Shortcut 5")},
    {"key_OutfitShortcut6", '6', KEY_MASK_CTRL, _("Outfit Shortcut 6")},
    {"key_OutfitShortcut7", '7', KEY_MASK_CTRL, _("Outfit Shortcut 7")},
    {"key_OutfitShortcut8", '8', KEY_MASK_CTRL, _("Outfit Shortcut 8")},
    {"key_OutfitShortcut9", '9', KEY_MASK_CTRL, _("Outfit Shortcut 9")},
    {"key_OutfitShortcut10", '0', KEY_MASK_CTRL, _("Outfit Shortcut 10")},
    {"key_EmoteShortcut1", '1', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 1)},
    {"key_EmoteShortcut2", '2', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 2)},
    {"key_EmoteShortcut3", '3', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 3)},
    {"key_EmoteShortcut4", '4', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 4)},
    {"key_EmoteShortcut5", '5', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 5)},
    {"key_EmoteShortcut6", '6', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 6)},
    {"key_EmoteShortcut7", '7', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 7)},
    {"key_EmoteShortcut8", '8', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 8)},
    {"key_EmoteShortcut9", '9', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 9)},
    {"key_EmoteShortcut10", '0', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 10)},
    {"key_EmoteShortcut11", '-', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 11)},
    {"key_EmoteShortcut12", '=', KEY_MASK_ALT, strprintf(_("Emote Shortcut %d"), 12)},
    {"key_Chat", Key::ENTER, 0, _("Toggle Chat")},
    {"key_ChatScrollUp", Key::PAGE_UP, 0, _("Scroll Chat Up")},
    {"key_ChatScrollDown", Key::PAGE_DOWN, 0, _("Scroll Chat Down")},
    {"key_ChatPrevTab", '[', 0, _("Previous Chat Tab")},
    {"key_ChatNextTab", ']', 0, _("Next Chat Tab")},
    {"key_OK", Key::SPACE, 0, _("Select OK")},
    {"key_PrevWin", Key::TAB, KEY_MASK_SHIFT || KEY_MASK_CTRL, _("Previous window")},
    {"key_NextWin", Key::TAB, KEY_MASK_CTRL, _("Next window")},
    {"key_Quit", Key::ESCAPE, 0, _("Quit")},
};

KeyboardConfig::KeyboardConfig()
{
    mDescs[0] = "None";
    mDescs[Key::ALT_GR] = "Alt GR";
    mDescs[Key::BACKSPACE] = "Backspace";
    mDescs[Key::CAPS_LOCK] = "Caps Lock";
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
    mDescs[Key::LEFT_ALT] = "Left Alt";
    mDescs[Key::LEFT_CONTROL] = "Left Ctrl";
    mDescs[Key::LEFT_META] = "Left Meta";
    mDescs[Key::LEFT_SHIFT] = "Left Shift";
    mDescs[Key::LEFT_SUPER] = "Left Super";
    mDescs[Key::NUM_LOCK] = "Num Lock";
    mDescs[Key::PAGE_DOWN] = "Page Down";
    mDescs[Key::PAGE_UP] = "Page Up";
    mDescs[Key::PAUSE] = "Pause";
    mDescs[Key::PRINT_SCREEN] = "Print Screen";
    mDescs[Key::RIGHT] = "Right";
    mDescs[Key::RIGHT_ALT] = "Right Alt";
    mDescs[Key::RIGHT_CONTROL] = "Right Ctrl";
    mDescs[Key::RIGHT_META] = "Right Meta";
    mDescs[Key::RIGHT_SHIFT] = "Right Shift";
    mDescs[Key::RIGHT_SUPER] = "Right Super";
    mDescs[Key::SCROLL_LOCK] = "Scroll Lock";
    mDescs[Key::SPACE] = "Space";
    mDescs[Key::TAB] = "Tab";
    mDescs[Key::UP] = "Up";

    // Initialize state map
    resetStates();
}

void KeyboardConfig::init()
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        mKey[i].key = keyData[i].key;
        mKey[i].mask = keyData[i].mask;
    }

    retrieve();
}

void KeyboardConfig::retrieve()
{
    for (int i = 0; i < KEY_TOTAL; i++)
    {
        KeyData conf = keyParse(config.getValue(keyData[i].configField, ""));
        if (conf.key)
            mKey[i] = conf;
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

bool KeyboardConfig::hasConflicts() const
{
    bool used[KEY_TOTAL];
    memset(used, 0, sizeof(used));

    for (int i = 0; i < KEY_TOTAL; i++)
    {
        int key = getKeyIndex(mKey[i]);
        if (key != KEY_NO_VALUE && used[key])
            return true;
        used[key] = 1;
    }
    return false;
}

void KeyboardConfig::keyPressed(gcn::KeyEvent &event)
{
    // Ignore consumed events
    if (event.isConsumed() || state != STATE_GAME)
        return;

    KeyData kd = keyConvert(event);

    int key = getKeyIndex(kd);

    if (key > -1)
    {
        mStates[key] = true;
        event.consume();
    }
}

void KeyboardConfig::keyReleased(gcn::KeyEvent &event)
{
    // Ignore consumed events
    if (event.isConsumed() || state != STATE_GAME)
        return;

    KeyData kd = keyConvert(event);

    int key = getKeyIndex(kd);

    if (key > -1)
    {
        mStates[key] = false;
        event.consume();
    }
}

int KeyboardConfig::getKeyIndex(KeyData key) const
{
    if (keyMatch(key, NULL_KEY))
        return KEY_NO_VALUE;

    for (int i = 0; i < KEY_TOTAL; i++)
    {
        if (keyMatch(key, mKey[i]))
            return i;
    }

    return KEY_NO_VALUE;
}

KeyData KeyboardConfig::getKeyData(int index) const
{
    if (index < 0)
        return NULL_KEY;
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

bool KeyboardConfig::keyMatch(KeyData data, KeyData ev) const
{
    return (ev.key == data.key || data.key == 0) && (ev.mask == data.mask);
}

std::string KeyboardConfig::keyString(KeyData data) const
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

    /*if (data.mask & KEY_MASK_SUPER)
        out << "Super+";*/

    // If it's printable, then do so
    if (data.key > 32 && data.key < 256)
        out << (char) data.key;
    // Otherwise, check our map
    else
    {
        KeyDescMap::const_iterator it = mDescs.find(data.key);
        if (it != mDescs.end())
            out << it->second;
        // Anything else, just quote the numeric value
        else
            out << "'" + toString(data.key) + "'";
    }

    return out.str();
}

KeyData KeyboardConfig::keyParse(std::string keyS) const
{
    KeyData kd = {0, 0};

    keyS = toLower(keyS);

    while (keyS.length())
    {
        if (strncasecmp(keyS.c_str(), "Shift+", 6) == 0)
        {
            kd.mask |= KEY_MASK_SHIFT;
            keyS.erase(0, 6);
        }
        else if (strncasecmp(keyS.c_str(), "Ctrl+", 5) == 0)
        {
            kd.mask |= KEY_MASK_CTRL;
            keyS.erase(0, 5);
        }
        else if (strncasecmp(keyS.c_str(), "Alt+", 4) == 0)
        {
            kd.mask |= KEY_MASK_ALT;
            keyS.erase(0, 4);
        }
        else if (strncasecmp(keyS.c_str(), "Meta+", 5) == 0)
        {
            kd.mask |= KEY_MASK_META;
            keyS.erase(0, 5);
        }
        /*else if (strncasecmp(keyS.c_str(), "Super+", 6) == 0)
        {
            kd.mask |= KEY_MASK_SUPER;
            keyS.erase(0, 6);
        }*/
        else
        {
            int temp;
            if (keyS.length() == 1)
                kd.key = keyS.at(0);
            else if (sscanf(keyS.c_str(), "'%d'", &temp) == 1)
                kd.key = temp;
            else
            {
                KeyDescMap::const_iterator it;

                for (it = mDescs.begin(); it != mDescs.end(); it++)
                {
                    temp = (*it).second.length();
                    if (temp != keyS.length())
                        continue;

                    if (strncasecmp(keyS.c_str(), (*it).second.c_str(), temp) == 0)
                    {
                        kd.key = (*it).first;
                        break;
                    }
                }
            }

            keyS.erase();
        }
    }

    return kd;
}

KeyData KeyboardConfig::keyConvert(gcn::KeyEvent &event) const
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

    /*if (event.isSuperPressed())
        ret.mask |= KEY_MASK_SUPER;*/

    // Make sure to always have lowercase letters
    if (event.isShiftPressed() && ret.key >= 65 && ret.key <= 90)
        ret.key += 32;

    // Convert the ctrl codes back to what the keyboard says
    if (event.isControlPressed())
    {
        if (ret.key == 0)
        {
            ret.key = '2';
        }
        else if (ret.key >= 1 && ret.key <= 26)
        {
            ret.key += 96;
        }
        else if (ret.key >= 27 && ret.key <= 31)
        {
            ret.key += 24;
        }
        else if (ret.key == 127)
        {
            ret.key = '8';
        }
    }

    return ret;
}

static Being::Type lastTarget = Being::UNKNOWN;

void KeyboardConfig::resetStates()
{
    memset(mStates, 0, sizeof(mStates));
}

bool KeyboardConfig::isKeyActive(int key) const
{
    return mStates[key];
}

void KeyboardConfig::processStates()
{
    parseWindows();

    if (isKeyActive(KEY_HIDE_WINDOWS))
    {
        statusWindow->setVisible(false);
        inventoryWindow->setVisible(false);
        skillDialog->setVisible(false);
        setupWindow->setVisible(false);
        equipmentWindow->setVisible(false);
        helpWindow->setVisible(false);
        debugWindow->setVisible(false);
#ifdef TMWSERV_SUPPORT
        guildWindow->setVisible(false);
        buddyWindow->setVisible(false);
#endif
        outfitWindow->setVisible(false);
    }

    parseMovement();

    parseTarget();

    if (isKeyActive(KEY_ATTACK))
        if (player_node->getTarget())
            player_node->attack(player_node->getTarget(), true);

    if (isKeyActive(KEY_PICKUP))
    {
        player_node->pickUp();
    }

    if (isKeyActive(KEY_SCREENSHOT))
    {
        Game::saveScreenshot();
    }

    if (isKeyActive(KEY_PATHFIND))
    {
        viewport->toggleDebugPath();
    }

    if (isKeyActive(KEY_TRADE))
    {
        // Toggle accepting of incoming trade requests
        unsigned int deflt = player_relations.getDefault();
        if (deflt & PlayerRelation::TRADE)
        {
            localChatTab->chatLog(_("Ignoring incoming trade requests"),
                        BY_SERVER);
            deflt &= ~PlayerRelation::TRADE;
        }
        else
        {
            localChatTab->chatLog(_("Accepting incoming trade requests"),
                        BY_SERVER);
            deflt |= PlayerRelation::TRADE;
        }

        player_relations.setDefault(deflt);
    }

    parseItemShortcut();
    parseEmoteShortcut();
    parseOutfitShortcut();

    if (isKeyActive(KEY_TOGGLE_CHAT))
    {
        chatWindow->requestChatFocus();
    }

    if (isKeyActive(KEY_SCROLL_CHAT_UP))
    {
        chatWindow->scroll(-DEFAULT_CHAT_WINDOW_SCROLL);
    }

    if (isKeyActive(KEY_SCROLL_CHAT_DOWN))
    {
        chatWindow->scroll(DEFAULT_CHAT_WINDOW_SCROLL);
    }

    if (isKeyActive(KEY_PREV_CHAT_TAB))
    {
        chatWindow->prevTab();
    }

    if (isKeyActive(KEY_NEXT_CHAT_TAB))
    {
        chatWindow->nextTab();
    }

    if (isKeyActive(KEY_SIT))
    {
        player_node->toggleSit();
    }

    if (isKeyActive(KEY_TALK))
    {
        if (!NPC::isTalking)
        {
            Being *target = player_node->getTarget();

            if (target && target->getType() == Being::NPC)
                dynamic_cast<NPC*>(target)->talk();
        }
    }

    if (isKeyActive(KEY_NEXT_WINDOW))
    {
        windowContainer->nextWindow();
    }
    else if (isKeyActive(KEY_PREV_WINDOW))
    {
        windowContainer->previousWindow();
    }

    if (isKeyActive(KEY_QUIT))
    {
        Game::quit();
    }
}

inline void KeyboardConfig::parseMovement()
{
    if (player_node->mAction == Being::DEAD && current_npc != 0)
        return;

    // Now get the adjusted direction (remove conflicting directions)
    int direction = 0;

    if (isKeyActive(KEY_MOVE_UP))
        direction |= Being::UP;
    else if (isKeyActive(KEY_MOVE_DOWN))
        direction |= Being::DOWN;

    if (isKeyActive(KEY_MOVE_LEFT))
        direction |= Being::LEFT;
    else if (isKeyActive(KEY_MOVE_RIGHT))
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
    if (isKeyActive(KEY_ATTACK) && lastTarget != Being::UNKNOWN)
    {
        Being *target = NULL;

        bool newTarget = isKeyActive(KEY_ATTACK);
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
}

inline void KeyboardConfig::parseTarget()
{
#ifdef TMWSERV_SUPPORT
    const Vector &pos = player_node->getPosition();
    const Uint16 x = (int) pos.x / 32;
    const Uint16 y = (int) pos.y / 32;
#else
    const Uint16 x = player_node->mX;
    const Uint16 y = player_node->mY;
#endif

    if (keyboard.isKeyActive(keyboard.KEY_TARGET_ATTACK))
    {
        Being *target = NULL;

        bool newTarget = !keyboard.isKeyActive(keyboard.KEY_TARGET_ATTACK);
        // A set target has highest priority
        if (!player_node->getTarget())
        {
            Uint16 targetX = x, targetY = y;
            // Only auto target Monsters
            target = beingManager->findNearestLivingBeing(targetX, targetY,
                     20, Being::MONSTER);
        }
        player_node->attack(target, newTarget);
    }

    Being::Type currentTarget = Being::UNKNOWN;

    if (isKeyActive(KEY_TARGET_MONSTER))
        currentTarget = Being::MONSTER;
    else if (isKeyActive(KEY_TARGET_NPC))
        currentTarget = Being::NPC;
    else if (isKeyActive(KEY_TARGET_PLAYER))
        currentTarget = Being::PLAYER;
    else lastTarget = Being::UNKNOWN; // Reset last target

    if (currentTarget == Being::UNKNOWN)
        return;

    Being *target = beingManager->findNearestLivingBeing(player_node, 20,
                                                         currentTarget);

    if (target && (target != player_node->getTarget() ||
            currentTarget != lastTarget))
    {
        player_node->setTarget(target);
        lastTarget = currentTarget;
    }
}

inline void KeyboardConfig::parseWindows()
{
    gcn::Window *requestedWindow = NULL;

    if (isKeyActive(KEY_WINDOW_HELP))
    // Needs special handling
    {
        if (helpWindow->isVisible())
            helpWindow->setVisible(false);
        else
        {
            helpWindow->loadHelp("index");
            helpWindow->requestMoveToTop();
        }
    }

    if (isKeyActive(KEY_WINDOW_STATUS))
        requestedWindow = statusWindow;

    if (isKeyActive(KEY_WINDOW_INVENTORY))
        requestedWindow = inventoryWindow;

    if (isKeyActive(KEY_WINDOW_EQUIPMENT))
        requestedWindow = equipmentWindow;

    if (isKeyActive(KEY_WINDOW_SKILL))
        requestedWindow = skillDialog;

    if (isKeyActive(KEY_WINDOW_MINIMAP))
        requestedWindow = minimap;

    if (isKeyActive(KEY_WINDOW_CHAT))
        requestedWindow = chatWindow;

    if (isKeyActive(KEY_WINDOW_SHORTCUT))
        requestedWindow = itemShortcutWindow;

    if (isKeyActive(KEY_WINDOW_SETUP))
        requestedWindow = setupWindow;

    if (isKeyActive(KEY_WINDOW_DEBUG))
        requestedWindow = debugWindow;

    if (isKeyActive(KEY_WINDOW_PARTY))
        requestedWindow = partyWindow;

    if (isKeyActive(KEY_WINDOW_EMOTE_SHORTCUT))
        requestedWindow = emoteShortcutWindow;

    if (isKeyActive(KEY_WINDOW_OUTFIT))
        requestedWindow = outfitWindow;

    if (requestedWindow)
    {
        requestedWindow->setVisible(!requestedWindow->isVisible());
        if (requestedWindow->isVisible())
        {
            requestedWindow->requestMoveToTop();
            //if (requestedWindow->isFocusable())
            //    requestedWindow->requestFocus();
        }
        else if (requestedWindow->isFocused())
        {
            gui->focusTop(true);
        }
    }
}

inline void KeyboardConfig::parseItemShortcut()
{
    // Checks if any item shortcut is pressed.
    for (int i = KEY_SHORTCUT_1; i <= KEY_SHORTCUT_12; i++)
    {
        if (isKeyActive(i))
            itemShortcut->useItem(i - KEY_SHORTCUT_1);
    }
}

inline void KeyboardConfig::parseEmoteShortcut()
{
    // Checks if any item shortcut is pressed.
    for (int i = KEY_EMOTE_1; i <= KEY_EMOTE_12; i++)
    {
        if (isKeyActive(i))
            emoteShortcut->useEmote(1 + i - KEY_EMOTE_1);
    }
}

inline void KeyboardConfig::parseOutfitShortcut()
{
    // Checks if any item shortcut is pressed.
    for (int i = KEY_OUTFIT_1; i <= KEY_OUTFIT_10; i++)
    {
        if (isKeyActive(i))
            outfitWindow->wearOutfit(i - KEY_OUTFIT_1);
    }
}
