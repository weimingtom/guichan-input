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

#include "being.h"

#include <cassert>
#include <cmath>

#include "animatedsprite.h"
#include "equipment.h"
#include "game.h"
#include "graphics.h"
#include "log.h"
#include "map.h"
#include "particle.h"
#include "sound.h"
#include "localplayer.h"

#include "resources/itemdb.h"
#include "resources/resourcemanager.h"
#include "resources/imageset.h"
#include "resources/iteminfo.h"

#include "gui/gui.h"
#include "gui/speechbubble.h"

#include "utils/dtor.h"
#include "utils/tostring.h"
#include "utils/xml.h"

namespace {
const bool debug_movement = true;
}

#define HAIR_FILE "hair.xml"

#include "utils/xml.h"

#define BEING_EFFECTS_FILE "effects.xml"

int Being::instances = 0;
ImageSet *Being::emotionSet = NULL;

Being::Being(int id, int job, Map *map):
    mEmotion(0), mEmotionTime(0),
    mAttackSpeed(350),
    mAction(STAND),
    mJob(job),
    mId(id),
    mSpriteDirection(DIRECTION_DOWN), mDirection(DOWN),
    mMap(NULL),
    mEquippedWeapon(NULL),
    mHairStyle(0),
    mHairColor(0),
    mGender(GENDER_UNSPECIFIED),
    mSpeechTime(0),
    mSprites(VECTOREND_SPRITE, NULL),
    mSpriteIDs(VECTOREND_SPRITE, 0),
    mSpriteColors(VECTOREND_SPRITE, ""),
    mWalkSpeed(100)
{
    setMap(map);

    mSpeechBubble = new SpeechBubble();

    if (instances == 0)
    {
        // Load the emotion set
        ResourceManager *rm = ResourceManager::getInstance();
        emotionSet = rm->getImageSet("graphics/sprites/emotions.png", 30, 32);
        if (!emotionSet)
            logger->error("Unable to load emotions!");
    }

    instances++;
}

Being::~Being()
{
    delete_all(mSprites);
    clearPath();

    for (   std::list<Particle *>::iterator i = mChildParticleEffects.begin();
            i != mChildParticleEffects.end();
            i++)
    {
        (*i)->kill();
    }

    setMap(NULL);

    instances--;

    if (instances == 0)
    {
        emotionSet->decRef();
        emotionSet = NULL;
    }

    delete mSpeechBubble;
}

void Being::setPosition(const Vector &pos)
{
    mPos = pos;
    mDest = pos;
}

void Being::adjustCourse(int srcX, int srcY, int dstX, int dstY)
{
    if (debug_movement)
        printf("%p adjustCourse(%d, %d, %d, %d)\n",
                (void*) this, srcX, srcY, dstX, dstY);

    mDest.x = dstX;
    mDest.y = dstY;

    // Find a path to the destination when it is at least a tile away
    if (mMap && fabsf((mDest - mPos).length()) > 32) {
        setPath(mMap->findPath((int) mPos.x / 32, (int) mPos.y / 32,
                               dstX / 32, dstY / 32, getWalkMask()));
    } else {
        setPath(Path());
    }

    // TODO: Evaluate the implementation of this method
    /*
    if (mX / 32 == dstX / 32 && mY / 32 == dstY / 32)
    {
        // The being is already on the last tile of the path.
        Path p;
        p.push_back(Position(dstX, dstY));
        setPath(p);
        return;
    }

    Path p1;
    int p1_size, p1_length;
    Uint16 *p1_dist;
    int onPath = -1;
    if (srcX / 32 == dstX / 32 && srcY / 32 == dstY / 32)
    {
        p1_dist = new Uint16[1];
        p1_size = 1;
        p1_dist[0] = 0;
        p1_length = 0;
    }
    else
    {
        p1 = mMap->findPath(srcX / 32, srcY / 32, dstX / 32, dstY / 32, getWalkMask());
        if (p1.empty())
        {
            // No path, but don't teleport since it could be user input.
            setPath(p1);
            return;
        }
        p1_size = p1.size();
        p1_dist = new Uint16[p1_size];
        int j = 0;
        // Remove last tile so that it can be replaced by the exact destination.
        p1.pop_back();
        for (Path::iterator i = p1.begin(), i_end = p1.end(); i != i_end; ++i)
        {
            // Get distance from source to tile i.
            p1_dist[j] = mMap->getMetaTile(i->x, i->y)->Gcost;
            // Check if the being is already walking on the path.
            if (i->x == mX / 32 && i->y == mY / 32)
            {
                onPath = j;
            }
            // Do not set any offset for intermediate steps.
            i->x = i->x * 32;
            i->y = i->y * 32;
            ++j;
        }
        p1_length = mMap->getMetaTile(dstX / 32, dstY / 32)->Gcost;
        p1_dist[p1_size - 1] = p1_length;
    }
    p1.push_back(Position(dstX, dstY));

    if (mX / 32 == srcX / 32 && mY / 32 == srcY / 32)
    {
        // The being is at the start of the path.
        setPath(p1);
        delete[] p1_dist;
        return;
    }

    if (onPath >= 0)
    {
        // The being is already on the path, but it needs to be slowed down.
        for (int j = onPath; j >= 0; --j)
        {
            p1.pop_front();
        }
        int r = p1_length - p1_dist[onPath];  // remaining length
        assert(r > 0);
        setPath(p1, p1_length * 1024 / r);
        delete[] p1_dist;
        return;
    }

    Path bestPath;
    int bestRating = -1, bestStart = 0, bestLength = 0;
    int j = 0;

    for (Path::iterator i = p1.begin(), i_end = p1.end(); i != i_end; ++i)
    {
        // Look if it is worth passing by tile i.
        Path p2 = mMap->findPath(mX / 32, mY / 32, i->x / 32, i->y / 32, getWalkMask());
        if (!p2.empty())
        {
            int l1 = mMap->getMetaTile(i->x / 32, i->y / 32)->Gcost;
            int l2 = p1_length - p1_dist[j];
            int r = l1 + l2 / 2; // TODO: tune rating formula
            assert(r > 0);
            if (bestRating < 0 || r < bestRating)
            {
                bestPath.swap(p2);
                bestRating = r;
                bestStart = j;
                bestLength = l1 + l2;
            }
        }
        ++j;
    }

    if (bestRating < 0)
    {
        // Unable to reach the path? Still, don't teleport since it could be
        // user input instead of server command.
        setPath(p1);
        delete[] p1_dist;
        return;
    }

    bestPath.pop_back();
    for (Path::iterator i = bestPath.begin(), i_end = bestPath.end(); i != i_end; ++i)
    {
        i->x = i->x * 32;
        i->y = i->y * 32;
    }

    // Concatenate paths.
    for (int j = bestStart; j > 0; --j)
    {
        p1.pop_front();
    }
    p1.splice(p1.begin(), bestPath);

    assert(bestLength > 0);
    setPath(p1, p1_length * 1024 / bestLength);
    delete[] p1_dist;
    */
}

void Being::adjustCourse(int srcX, int srcY)
{
    if (debug_movement)
        printf("%p adjustCourse(%d, %d)\n", (void*) this, srcX, srcY);

    if (!mPath.empty())
        adjustCourse(srcX, srcY, mPath.back().x * 32, mPath.back().y * 32);
}

void Being::setDestination(int destX, int destY)
{
    if (debug_movement)
        printf("%p setDestination(%d, %d)\n", (void*) this, destX, destY);

    adjustCourse((int) mPos.x, (int) mPos.y, destX, destY);
}

void Being::clearPath()
{
    mPath.clear();
}

void Being::setPath(const Path &path)
{
    std::cout << this << " New path: " << path << std::endl;
    mPath = path;
}

void Being::setHairStyle(int style, int color)
{
    mHairStyle = style < 0 ? mHairStyle : style % getHairStylesNr();
    mHairColor = color < 0 ? mHairColor : color % getHairColorsNr();
}

void Being::setSprite(int slot, int id, const std::string &color)
{
    assert(slot >= BASE_SPRITE && slot < VECTOREND_SPRITE);
    mSpriteIDs[slot] = id;
    mSpriteColors[slot] = color;
}

void Being::setSpeech(const std::string &text, Uint32 time)
{
    mSpeech = text;
    mSpeechTime = 500;
}

void Being::takeDamage(int amount)
{
    gcn::Font *font;
    std::string damage = amount ? toString(amount) : "miss";

    // Selecting the right color
    if (damage == "miss")
    {
        font = hitYellowFont;
    }
    else
    {
        // Hit particle effect
        controlParticle(particleEngine->addEffect(
                    "graphics/particles/hit.particle.xml", mPos.x, mPos.y));

        if (getType() == MONSTER)
        {
            font = hitBlueFont;
        }
        else
        {
            font = hitRedFont;
        }
    }

    // Show damage number
    particleEngine->addTextSplashEffect(damage, 255, 255, 255, font,
                                        (int) mPos.x + 16,
                                        (int) mPos.y + 16);
}

void Being::handleAttack()
{
    setAction(Being::ATTACK);
}

void Being::setMap(Map *map)
{
    // Remove sprite from potential previous map
    if (mMap)
    {
        mMap->removeSprite(mSpriteIterator);
    }

    mMap = map;

    // Add sprite to potential new map
    if (mMap)
    {
        mSpriteIterator = mMap->addSprite(this);
    }

    // Clear particle effect list because child particles became invalid
    mChildParticleEffects.clear();
}

void Being::controlParticle(Particle *particle)
{
    if (particle)
    {
        // The effect may not die without the beings permission or we segfault
        particle->disableAutoDelete();
        mChildParticleEffects.push_back(particle);
    }
}

void Being::setAction(Action action, int attackType)
{
    SpriteAction currentAction = ACTION_INVALID;
    switch (action)
    {
        case WALK:
            currentAction = ACTION_WALK;
            break;
        case SIT:
            currentAction = ACTION_SIT;
            break;
        case ATTACK:
            if (mEquippedWeapon)
            {
                currentAction = mEquippedWeapon->getAttackType();
            }
            else {
                currentAction = ACTION_ATTACK;
            }
            for (int i = 0; i < VECTOREND_SPRITE; i++)
            {
                if (mSprites[i])
                {
                    mSprites[i]->reset();
                }
            }
            break;
        case HURT:
            //currentAction = ACTION_HURT;  // Buggy: makes the player stop
                                            // attacking and unable to attack
                                            // again until he moves
            break;
        case DEAD:
            currentAction = ACTION_DEAD;
            break;
        case STAND:
            currentAction = ACTION_STAND;
            break;
    }

    if (currentAction != ACTION_INVALID)
    {
        for (int i = 0; i < VECTOREND_SPRITE; i++)
        {
            if (mSprites[i])
            {
                mSprites[i]->play(currentAction);
            }
        }
        mAction = action;
    }
}


void Being::setDirection(Uint8 direction)
{
    if (mDirection == direction)
        return;

    // if the direction does not change much, keep the common component
    int mFaceDirection = mDirection & direction;
    if (!mFaceDirection)
        mFaceDirection = direction;
    mDirection = direction;

    SpriteDirection dir;
    if (mFaceDirection & UP)
        dir = DIRECTION_UP;
    else if (mFaceDirection & DOWN)
        dir = DIRECTION_DOWN;
    else if (mFaceDirection & RIGHT)
        dir = DIRECTION_RIGHT;
    else
        dir = DIRECTION_LEFT;
    mSpriteDirection = dir;

    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (mSprites[i] != NULL)
            mSprites[i]->setDirection(dir);
    }
}

void Being::logic()
{
    const Vector dest = (mPath.empty()) ?
        mDest : Vector(mPath.front().x * 32 + 16,
                       mPath.front().y * 32 + 16);

    Vector dir = dest - mPos;
    const float length = dir.length();

    // When we're over 2 pixels from our destination, move to it
    // TODO: Should be possible to make it even pixel exact, but this solves
    //       the jigger caused by moving too far.
    if (length > 2.0f) {
        const float speed = mWalkSpeed / 100.0f;
        mPos += dir / (length / speed);

        if (mAction != WALK)
            setAction(WALK);

        // Update the player sprite direction
        int direction = 0;
        const float dx = std::abs(dir.x);
        const float dy = std::abs(dir.y);
        if (dx > dy)
            direction |= (dir.x > 0) ? RIGHT : LEFT;
        else
            direction |= (dir.y > 0) ? DOWN : UP;
        setDirection(direction);
    }
    else if (!mPath.empty()) {
        // TODO: Pop as soon as there is a direct unblocked line to the next
        //       point on the path.
        mPath.pop_front();
    } else if (mAction == WALK) {
        setAction(STAND);
    }

    // Reduce the time that speech is still displayed
    if (mSpeechTime > 0)
        mSpeechTime--;

    if (mEmotion != 0)
    {
        mEmotionTime--;
        if (mEmotionTime == 0) {
            mEmotion = 0;
        }
    }

    // Update sprite animations
    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (mSprites[i] != NULL)
        {
            mSprites[i]->update(tick_time * 10);
        }
    }

    // Update particle effects
    for (std::list<Particle *>::iterator i = mChildParticleEffects.begin();
         i != mChildParticleEffects.end();)
    {
        (*i)->setPosition(mPos.x, mPos.y);
        if ((*i)->isExtinct())
        {
            (*i)->kill();
            i = mChildParticleEffects.erase(i);
        }
        else {
            i++;
        }
    }
}

void Being::draw(Graphics *graphics, int offsetX, int offsetY) const
{
    int px = (int) mPos.x + offsetX;
    int py = (int) mPos.y + offsetY;

    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (mSprites[i] != NULL)
        {
            // TODO: Eventually, we probably should fix all sprite offsets so
            //       that this translation isn't necessary anymore.
            mSprites[i]->draw(graphics, px - 16, py - 32);
        }
    }
}

void Being::drawEmotion(Graphics *graphics, int offsetX, int offsetY)
{
    if (!mEmotion)
        return;

    const int px = (int) mPos.x + offsetX + 3;
    const int py = (int) mPos.y + offsetY - 60;
    const int emotionIndex = mEmotion - 1;

    if (emotionIndex >= 0 && emotionIndex < (int) emotionSet->size())
        graphics->drawImage(emotionSet->get(emotionIndex), px, py);
}

void Being::drawSpeech(Graphics *graphics, int offsetX, int offsetY)
{
    int px = (int) mPos.x + offsetX;
    int py = (int) mPos.y + offsetY;

    // Draw speech above this being
    if (mSpeechTime > 0)
    {
        mSpeechBubble->setPosition(px - 50, py - 80 - (mSpeechBubble->getNumRows()*14) );
        mSpeechBubble->setText( mSpeech );
        mSpeechBubble->setVisible(true);
    }
    else if (mSpeechTime == 0)
    {
        mSpeechBubble->setVisible(false);
    }
}

Being::Type Being::getType() const
{
    return UNKNOWN;
}

int Being::getWidth() const
{
    if (mSprites[BASE_SPRITE])
    {
        return mSprites[BASE_SPRITE]->getWidth();
    }
    else {
        return 0;
    }
}

int Being::getHeight() const
{
    if (mSprites[BASE_SPRITE])
    {
        return mSprites[BASE_SPRITE]->getHeight();
    }
    else {
        return 0;
    }
}




static int hairStylesNr;
static int hairColorsNr;
static std::vector<std::string> hairColors;

static void
initializeHair(void);

int
Being::getHairStylesNr(void)
{
    initializeHair();
    return hairStylesNr;
}

int
Being::getHairColorsNr(void)
{
    initializeHair();
    return hairColorsNr;
}

std::string
Being::getHairColor(int index)
{
    initializeHair();
    if (index < 0 || index >= hairColorsNr)
        return "#000000";

    return hairColors[index];
}

static bool hairInitialized = false;

static void
initializeHair(void)
{
    if (hairInitialized)
        return;

    // Hairstyles are encoded as negative numbers.  Count how far negative we can go.
    int hairstylesCtr = -1;
    while (ItemDB::get(hairstylesCtr).getSprite(GENDER_MALE) != "error.xml")
        --hairstylesCtr;

    hairStylesNr = -hairstylesCtr; // done.
    if (hairStylesNr == 0)
        hairStylesNr = 1; // No hair style -> no hair

    hairColorsNr = 0;

    XML::Document doc(HAIR_FILE);
    xmlNodePtr root = doc.rootNode();

    if (!root || !xmlStrEqual(root->name, BAD_CAST "colors"))
    {
        logger->log("Error loading being hair configuration file");
    } else {
        for_each_xml_child_node(node, root)
        {
            if (xmlStrEqual(node->name, BAD_CAST "color"))
            {
                int index = atoi(XML::getProperty(node, "id", "-1").c_str());
                std::string value = XML::getProperty(node, "value", "");

                if (index >= 0 && value != "") {
                    if (index >= hairColorsNr) {
                        hairColorsNr = index + 1;
                        hairColors.resize(hairColorsNr, "#000000");
                    }
                    hairColors[index] = value;
                }
            }
        }
    } // done initializing

    if (hairColorsNr == 0) { // No colours -> black only
        hairColorsNr = 1;
        hairColors.resize(hairColorsNr, "#000000");
    }

    hairInitialized = 1;
}



struct EffectDescription {
    std::string mGFXEffect;
    std::string mSFXEffect;
};

static EffectDescription *default_effect = NULL;
static std::map<int, EffectDescription *> effects;
static bool effects_initialized = false;

static EffectDescription *
getEffectDescription(xmlNodePtr node, int *id)
{
    EffectDescription *ed = new EffectDescription;

    *id = atoi(XML::getProperty(node, "id", "-1").c_str());
    ed->mSFXEffect = XML::getProperty(node, "audio", "");
    ed->mGFXEffect = XML::getProperty(node, "particle", "");

    return ed;
}

static EffectDescription *
getEffectDescription(int effectId)
{
    if (!effects_initialized)
    {
        XML::Document doc(BEING_EFFECTS_FILE);
        xmlNodePtr root = doc.rootNode();

        if (!root || !xmlStrEqual(root->name, BAD_CAST "being-effects"))
        {
            logger->log("Error loading being effects file: "
                    BEING_EFFECTS_FILE);
            return NULL;
        }

        for_each_xml_child_node(node, root)
        {
            int id;

            if (xmlStrEqual(node->name, BAD_CAST "effect"))
            {
                EffectDescription *EffectDescription =
                    getEffectDescription(node, &id);
                effects[id] = EffectDescription;
            } else if (xmlStrEqual(node->name, BAD_CAST "default"))
            {
                EffectDescription *EffectDescription =
                    getEffectDescription(node, &id);

                if (default_effect)
                    delete default_effect;

                default_effect = EffectDescription;
            }
        }

        effects_initialized = true;
    } // done initializing

    EffectDescription *ed = effects[effectId];

    if (!ed)
        return default_effect;
    else
        return ed;
}

void
Being::internalTriggerEffect(int effectId, bool sfx, bool gfx)
{
    logger->log("Special effect #%d on %s", effectId,
                getId() == player_node->getId() ? "self" : "other");

    EffectDescription *ed = getEffectDescription(effectId);

    if (!ed) {
        logger->log("Unknown special effect and no default recorded");
        return;
    }

    if (gfx && ed->mGFXEffect != "") {
        Particle *selfFX;

        selfFX = particleEngine->addEffect(ed->mGFXEffect, 0, 0);
        controlParticle(selfFX);
    }

    if (sfx && ed->mSFXEffect != "") {
        sound.playSfx(ed->mSFXEffect);
    }
}
