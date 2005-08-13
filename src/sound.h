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

#ifndef _TMW_SOUND_H
#define _TMW_SOUND_H

#include <SDL_mixer.h>

/** Sound engine
 *
 * \ingroup CORE
 */
class Sound {
    public:
        /**
         * Constructor.
         */
        Sound();

        /**
         * Destructor.
         */
        ~Sound();

        /**
         * Installs the sound engine.
         */
        void init();

        /**
         * Logs various info about sound device.
         */
        void info();

        /**
         * Removes all sound functionalities.
         */
        void close();

        /**
         * Starts background music.
         *
         * \param in   Full path to file
         * \param loop The number of times the song is played (-1 = infinite)
         */
        void playMusic(const char *path, int loop = -1);

        /**
         * Stops currently running background music track.
         */
        void stopMusic();

        /**
         * Fades in background music.
         *
         * \param in   Full path to file
         * \param loop The number of times the song is played (-1 = infinite)
         * \param ms   Duration of fade-in effect (ms)
         */
        void fadeInMusic(const char *path, int loop = -1, int ms = 2000);

        /**
         * Fades out currently running background music track.
         *
         * \param ms   Duration of fade-out effect (ms)
         */
        void fadeOutMusic(int ms);

        /**
         * Sets music volume.
         * 
         * \param volume Volume value
         */
        void setMusicVolume(int volume);

        /**
         * Sets sfx volume.
         *
         * \param volume Volume value
         */
        void setSfxVolume(int volume);

        /**
         * Plays an item.
         *
         * \param path     Full path to file
         */
        void playSfx(const char *path);

    private:
        bool installed;

        int sfxVolume;
        int musicVolume;

        Mix_Music *music;
};

#endif
