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

#include "beinghandler.h"

#include <SDL_types.h>

#include "messagein.h"
#include "protocol.h"

#include "../being.h"
#include "../beingmanager.h"
#include "../game.h"
#include "../localplayer.h"
#include "../log.h"
#include "../main.h"
#include "../particle.h"
#include "../sound.h"

#include "../gui/ok_dialog.h"

#include "../utils/gettext.h"

#include "gameserver/player.h"

const int EMOTION_TIME = 150;    /**< Duration of emotion icon */

BeingHandler::BeingHandler()
{
    static const Uint16 _messages[] = {
        //SMSG_BEING_VISIBLE,
        //SMSG_BEING_MOVE,
        //SMSG_BEING_REMOVE,
        //SMSG_BEING_ACTION,
        //SMSG_BEING_LEVELUP,
        //SMSG_BEING_EMOTION,
        //SMSG_BEING_CHANGE_LOOKS,
        //SMSG_BEING_NAME_RESPONSE,
        //SMSG_PLAYER_UPDATE_1,
        //SMSG_PLAYER_UPDATE_2,
        //SMSG_PLAYER_MOVE,
        //0x0119,
        GPMSG_BEING_ATTACK,
        GPMSG_BEING_ENTER,
        GPMSG_BEING_LEAVE,
        GPMSG_BEINGS_MOVE,
        GPMSG_BEINGS_DAMAGE,
        GPMSG_BEING_ACTION_CHANGE,
        GPMSG_BEING_LOOKS_CHANGE,
        0
    };
    handledMessages = _messages;
}

void BeingHandler::handleMessage(MessageIn &msg)
{
    /*
    Uint32 id;
    Uint16 job, speed;
    Uint16 headBottom, headTop, headMid;
    Sint16 param1;
    Sint8 type;
    Being *srcBeing, *dstBeing;
    */

    switch (msg.getId())
    {
        case GPMSG_BEING_ENTER:
            handleBeingEnterMessage(msg);
            break;
        case GPMSG_BEING_LEAVE:
            handleBeingLeaveMessage(msg);
            break;
        case GPMSG_BEINGS_MOVE:
            handleBeingsMoveMessage(msg);
            break;
        case GPMSG_BEING_ATTACK:
            handleBeingAttackMessage(msg);
            break;
        case GPMSG_BEINGS_DAMAGE:
            handleBeingsDamageMessage(msg);
            break;
        case GPMSG_BEING_ACTION_CHANGE:
            handleBeingActionChangeMessage(msg);
            break;
        case GPMSG_BEING_LOOKS_CHANGE:
            handleBeingLooksChangeMessage(msg);
            break;


        /*
        case SMSG_BEING_VISIBLE:
        case SMSG_BEING_MOVE:
            // Information about a being in range
            id = msg.readInt32();
            speed = msg.readInt16();
            msg.readInt16();  // unknown
            msg.readInt16();  // unknown
            msg.readInt16();  // option
            job = msg.readInt16();  // class

            dstBeing = beingManager->findBeing(id);

            if (!dstBeing)
            {
                // Being with id >= 110000000 and job 0 are better
                // known as ghosts, so don't create those.
                if (job == 0 && id >= 110000000)
                {
                    break;
                }

                dstBeing = beingManager->createBeing(id, job);
            }
            else if (msg.getId() == 0x0078)
            {
                dstBeing->clearPath();
                dstBeing->mFrame = 0;
                dstBeing->mWalkTime = tick_time;
                dstBeing->setAction(Being::STAND);
            }

            // Prevent division by 0 when calculating frame
            if (speed == 0) { speed = 150; }

            dstBeing->setWalkSpeed(speed);
            dstBeing->mJob = job;
            dstBeing->setHairStyle(msg->readInt16());
            dstBeing->setSprite(
                    Being::WEAPON_SPRITE, msg->readInt16());
            dstBeing->setSprite(
                    Being::BOTTOMCLOTHES_SPRITE, msg->readInt16());

            if (msg.getId() == SMSG_BEING_MOVE)
            {
                msg.readInt32(); // server tick
            }

            msg->readInt16();  // shield
            headTop = msg->readInt16();
            headMid = msg->readInt16();
            dstBeing->setSprite(Being::HAT_SPRITE, headTop);
            dstBeing->setSprite(Being::TOPCLOTHES_SPRITE, headMid);
            dstBeing->setHairColor(msg->readInt16());
            msg->readInt16();  // unknown
            msg->readInt16();  // head dir
            msg->readInt16();  // guild
            msg->readInt16();  // unknown
            msg->readInt16();  // unknown
            msg->readInt16();  // manner
            msg->readInt16();  // karma
            msg->readInt8();   // unknown
            dstBeing->setSex(1 - msg->readInt8());   // sex

            if (msg.getId() == SMSG_BEING_MOVE)
            {
                //Uint16 srcX, srcY, dstX, dstY;
                //msg.readCoordinatePair(srcX, srcY, dstX, dstY);
                //dstBeing->setAction(Being::STAND);
                //dstBeing->mX = srcX;
                //dstBeing->mY = srcY;
                //dstBeing->setDestination(dstX, dstY);
            }
            else
            {
                //Uint8 dir;
                //msg->readCoordinates(dstBeing->mX, dstBeing->mY, dir);
                //dstBeing->setDirection(dir);
            }

            msg.readInt8();   // unknown
            msg.readInt8();   // unknown
            msg.readInt8();   // unknown / sit
            break;

        case SMSG_BEING_REMOVE:
            // A being should be removed or has died
            dstBeing = beingManager->findBeing(msg.readInt32());

            if (!dstBeing)
                break;

            if (msg.readInt8() == 1)
            {
                dstBeing->setAction(Being::DEAD);
            }
            else
            {
                beingManager->destroyBeing(dstBeing);
            }

            if (dstBeing == player_node->getTarget())
            {
                player_node->stopAttack();
            }
            break;

        case SMSG_BEING_ACTION:
            srcBeing = beingManager->findBeing(msg.readInt32());
            dstBeing = beingManager->findBeing(msg.readInt32());
            msg.readInt32();   // server tick
            msg.readInt32();   // src speed
            msg.readInt32();   // dst speed
            param1 = msg.readInt16();
            msg.readInt16();  // param 2
            type = msg.readInt8();
            msg.readInt16();  // param 3

            switch (type)
            {
                case 0: // Damage
                    if (dstBeing) {
                        dstBeing->takeDamage(param1);
                    }
                    if (srcBeing) {
                        srcBeing->handleAttack(dstBeing, param1);
                    }
                    break;

                case 2: // Sit
                    if (srcBeing) {
                        srcBeing->mFrame = 0;
                        srcBeing->setAction(Being::SIT);
                    }
                    break;

                case 3: // Stand up
                    if (srcBeing) {
                        srcBeing->mFrame = 0;
                        srcBeing->setAction(Being::STAND);
                    }
                    break;
            }
            break;

        case SMSG_BEING_LEVELUP:
            id = (Uint32) msg->readInt32();

            if (id == player_node->getId()) {
                logger->log("Level up");
                sound.playSfx("sfx/levelup.ogg");
            }
            else {
                logger->log("Someone else went level up");
            }
            Particle *levelupFX;
            if (msg->readInt32() == 0) { // type
                levelupFX = particleEngine->addEffect(
                        "graphics/particles/levelup.particle.xml", 0, 0);
            }
            else {
                levelupFX = particleEngine->addEffect(
                        "graphics/particles/skillup.particle.xml", 0, 0);
            }
            beingManager->findBeing(id)->controlParticle(levelupFX);
            break;

        case SMSG_BEING_EMOTION:
            if (!(dstBeing = beingManager->findBeing(msg.readInt32())))
            {
                break;
            }

            dstBeing->mEmotion = msg.readInt8();
            dstBeing->mEmotionTime = EMOTION_TIME;
            break;

        case SMSG_BEING_CHANGE_LOOKS:
        {
            if (!(dstBeing = beingManager->findBeing(msg.readInt32())))
            {
                break;
            }

            int type = msg.readInt8();
            int id = msg.readInt8();

            switch (type) {
                case 1:
                    dstBeing->setHairStyle(id);
                    break;
                case 2:
                    dstBeing->setSprite(Being::WEAPON_SPRITE, id);
                    break;
                case 3:     // Change lower headgear for eAthena, pants for us
                    dstBeing->setSprite(
                            Being::BOTTOMCLOTHES_SPRITE, id);
                    break;
                case 4:     // Change upper headgear for eAthena, hat for us
                    dstBeing->setSprite(
                            Being::HAT_SPRITE, id);
                    break;
                case 5:     // Change middle headgear for eathena, armor for us
                     dstBeing->setSprite(
                            Being::TOPCLOTHES_SPRITE, id);
                    break;
                case 6:
                    dstBeing->setHairColor(id);
                    break;
                default:
                    logger->log("SMSG_BEING_CHANGE_LOOKS: unsupported type: "
                            "%d, id: %d", type, id);
                    break;
            }
        }
            break;

        case SMSG_BEING_NAME_RESPONSE:
            if ((dstBeing = beingManager->findBeing(msg.readInt32())))
            {
                dstBeing->setName(msg.readString(24));
            }
            break;

        case SMSG_PLAYER_UPDATE_1:
        case SMSG_PLAYER_UPDATE_2:
        case SMSG_PLAYER_MOVE:
            // An update about a player, potentially including movement.
            id = msg.readInt32();
            speed = msg.readInt16();
            msg.readInt16();  // option 1
            msg.readInt16();  // option 2
            msg.readInt16();  // option
            job = msg.readInt16();

            dstBeing = beingManager->findBeing(id);

            if (!dstBeing)
            {
                dstBeing = beingManager->createBeing(id, job);
            }

            dstBeing->setWalkSpeed(speed);
            dstBeing->mJob = job;
            dstBeing->setHairStyle(msg->readInt16());
            dstBeing->setSprite(
                    Being::WEAPON_SPRITE, msg->readInt16());
            msg->readInt16();  // item id 2
            headBottom = msg->readInt16();

            if (msg.getId() == SMSG_PLAYER_MOVE)
            {
                msg.readInt32(); // server tick
            }

            headTop = msg.readInt16();
            headMid = msg.readInt16();
            dstBeing->setHairColor(msg.readInt16());
            msg.readInt16();  // unknown
            msg.readInt16();  // head dir
            msg.readInt32();  // guild
            msg.readInt32();  // emblem
            msg.readInt16();  // manner
            msg.readInt8();   // karma
            dstBeing->setSex(1 - msg.readInt8());   // sex
            dstBeing->setSprite(
                    Being::BOTTOMCLOTHES_SPRITE, headBottom);
            dstBeing->setSprite(Being::HAT_SPRITE, headTop);
            dstBeing->setSprite(Being::TOPCLOTHES_SPRITE, headMid);

            if (msg.getId() == SMSG_PLAYER_MOVE)
            {
                //Uint16 srcX, srcY, dstX, dstY;
                //msg.readCoordinatePair(srcX, srcY, dstX, dstY);
                //dstBeing->mX = srcX;
                //dstBeing->mY = srcY;
                //dstBeing->setDestination(dstX, dstY);
            }
            else
            {
                //Uint8 dir;
                //msg->readCoordinates(dstBeing->mX, dstBeing->mY, dir);
                //dstBeing->setDirection(dir);
            }

            msg.readInt8();   // unknown
            msg.readInt8();   // unknown

            if (msg.getId() == SMSG_PLAYER_UPDATE_1)
            {
                if (msg.readInt8() == 2)
                {
                    dstBeing->setAction(Being::SIT);
                }
            }
            else if (msg.getId() == SMSG_PLAYER_MOVE)
            {
                msg.readInt8(); // unknown
            }

            msg.readInt8();   // Lv
            msg.readInt8();   // unknown

            dstBeing->mWalkTime = tick_time;
            dstBeing->mFrame = 0;
            break;

        case 0x0119:
            // Change in players look
            logger->log("0x0119 %i %i %i %x %i", msg->readInt32(),
                    msg->readInt16(), msg->readInt16(), msg->readInt16(),
                    msg->readInt8());
            break;
        */
    }
}

static void handleLooks(Player *being, MessageIn &msg)
{
    // Order of sent slots. Has to be in sync with the server code.
    static int const nb_slots = 4;
    static int const slots[nb_slots] =
        { Being::WEAPON_SPRITE, Being::HAT_SPRITE, Being::TOPCLOTHES_SPRITE,
          Being::BOTTOMCLOTHES_SPRITE };

    int mask = msg.readInt8();

    if (mask & (1 << 7))
    {
        // The equipment has to be cleared first.
        for (int i = 0; i < nb_slots; ++i)
        {
            being->setSprite(slots[i], 0);
        }
    }

    // Fill slots enumerated by the bitmask.
    for (int i = 0; i < nb_slots; ++i)
    {
        if (!(mask & (1 << i))) continue;
        int id = msg.readInt16();
        being->setSprite(slots[i], id);
    }
}

void
BeingHandler::handleBeingEnterMessage(MessageIn &msg)
{
    int type = msg.readInt8();
    int id = msg.readInt16();
    Being::Action action = (Being::Action)msg.readInt8();
    int px = msg.readInt16();
    int py = msg.readInt16();
    Being *being;

    switch (type)
    {
        case OBJECT_PLAYER:
        {
            std::string name = msg.readString();
            if (player_node->getName() == name)
            {
                being = player_node;
                being->setId(id);
            }
            else
            {
                being = beingManager->createBeing(id, type, 0);
                being->setName(name);
            }
            Player *p = static_cast< Player * >(being);
            int hs = msg.readInt8(), hc = msg.readInt8();
            p->setHairStyle(hs, hc);
            p->setGender(msg.readInt8() == GENDER_MALE ? GENDER_MALE : GENDER_FEMALE);
            handleLooks(p, msg);
        } break;

        case OBJECT_MONSTER:
        case OBJECT_NPC:
        {
            int subtype = msg.readInt16();
            being = beingManager->createBeing(id, type, subtype);
        } break;

        default:
            return;
    }

    being->mX = px;
    being->mY = py;
    being->setDestination(px, py);
    being->setAction(action);
}

void BeingHandler::handleBeingLeaveMessage(MessageIn &msg)
{
    Being *being = beingManager->findBeing(msg.readInt16());
    if (!being) return;

    beingManager->destroyBeing(being);
}

void BeingHandler::handleBeingsMoveMessage(MessageIn &msg)
{
    while (msg.getUnreadLength())
    {
        int id = msg.readInt16();
        int flags = msg.readInt8();
        Being *being = beingManager->findBeing(id);
        int sx = 0, sy = 0, dx = 0, dy = 0, speed = 0;
        if (flags & MOVING_POSITION)
        {
            Uint16 sx2, sy2;
            msg.readCoordinates(sx2, sy2);
            sx = sx2 * 32 + 16;
            sy = sy2 * 32 + 16;
            speed = msg.readInt8();
        }
        if (flags & MOVING_DESTINATION)
        {
            dx = msg.readInt16();
            dy = msg.readInt16();
            if (!(flags & MOVING_POSITION))
            {
                sx = dx;
                sy = dy;
            }
        }
        if (!being || !(flags & (MOVING_POSITION | MOVING_DESTINATION)))
        {
            continue;
        }
        if (speed)
        {
            being->setWalkSpeed(speed * 10);
        }
        if (abs(being->mX - sx) + abs(being->mY - sy) > 4 * 32)
        {
            // Too large a desynchronization.
            being->mX = sx;
            being->mY = sy;
            being->setDestination(dx, dy);
        }
        else if (!(flags & MOVING_POSITION))
        {
            being->setDestination(dx, dy);
        }
        else if (!(flags & MOVING_DESTINATION))
        {
            being->adjustCourse(sx, sy);
        }
        else
        {
            being->adjustCourse(sx, sy, dx, dy);
        }
    }
}

void BeingHandler::handleBeingAttackMessage(MessageIn &msg)
{
    Being *being = beingManager->findBeing(msg.readInt16());
    int direction = msg.readInt8();
    int attackType = msg.readInt8();

    if (!being) return;

    switch (direction)
    {
        case DIRECTION_UP: being->setDirection(Being::UP); break;
        case DIRECTION_DOWN: being->setDirection(Being::DOWN); break;
        case DIRECTION_LEFT: being->setDirection(Being::LEFT); break;
        case DIRECTION_RIGHT: being->setDirection(Being::RIGHT); break;
    }

    logger->log("Attacktype: %d", attackType);

    being->setAction(Being::ATTACK, attackType);
}

void BeingHandler::handleBeingsDamageMessage(MessageIn &msg)
{
    while (msg.getUnreadLength())
    {
        Being *being = beingManager->findBeing(msg.readInt16());
        int damage = msg.readInt16();
        if (being)
        {
            being->takeDamage(damage);
        }
    }
}

void BeingHandler::handleBeingActionChangeMessage(MessageIn &msg)
{
    Being* being = beingManager->findBeing(msg.readInt16());
    Being::Action action = (Being::Action) msg.readInt8();
    if (!being) return;

    being->setAction(action);

    if (action == Being::DEAD && being==player_node)
    {
        static char const *const deadMsg[] =
        {
            _("You are dead."),
            _("We regret to inform you that your character was killed in battle."),
            _("You are not that alive anymore."),
            _("The cold hands of the grim reaper are grabbing for your soul."),
            _("Game Over!"),
            _("No, kids. Your character did not really die. It... err... went to a better place."),
            _("Your plan of breaking your enemies weapon by bashing it with your throat failed."),
            _("I guess this did not run too well."),
            _("Do you want your possessions identified?"), // Nethack reference
            _("Sadly, no trace of you was ever found..."), // Secret of Mana reference
            _("Annihilated."), // Final Fantasy VI reference
            _("Looks like you got your head handed to you."), //Earthbound reference
            _("You screwed up again, dump your body down the tubes and get you another one.") // Leisure Suit Larry 1 Reference

        };
        std::string message(deadMsg[rand()%13]);
        message.append(_(" Press OK to respawn"));
        OkDialog *dlg = new OkDialog(_("You died"), message);
        dlg->addActionListener(&(Net::GameServer::Player::respawnListener));
    }
}

void BeingHandler::handleBeingLooksChangeMessage(MessageIn &msg)
{
    Being *being = beingManager->findBeing(msg.readInt16());
    if (!being || being->getType() != Being::PLAYER) return;
    handleLooks(static_cast< Player * >(being), msg);
}

