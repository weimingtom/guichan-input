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

#include "soundeffect.h"

SoundEffect::SoundEffect(const std::string &idPath, Mix_Chunk *soundEffect):
    Resource(idPath),
    mChunk(soundEffect)
{
}

SoundEffect::~SoundEffect()
{
    Mix_FreeChunk(mChunk);
}

SoundEffect*
SoundEffect::load(void *buffer, unsigned int bufferSize,
                  const std::string &idPath)
{
    // Load the raw file data from the buffer in an RWops structure
    SDL_RWops *rw = SDL_RWFromMem(buffer, bufferSize);

    // Use Mix_LoadWAV_RW to load the raw music data
    Mix_Chunk *tmpSoundEffect = Mix_LoadWAV_RW(rw, 0);

    // Now free the SDL_RWops data
    SDL_FreeRW(rw);

    return new SoundEffect(idPath, tmpSoundEffect);
}

bool
SoundEffect::play(int loops, int volume)
{
    Mix_VolumeChunk(mChunk, volume);

    return Mix_PlayChannel(-1, mChunk, loops) != -1;
}
