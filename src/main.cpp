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

#include "main.h"
#include "gui/char_server.h"
#include "gui/char_select.h"
#include "gui/inventory.h"
#include "gui/ok_dialog.h"
#include "sound.h"
#include "graphics.h"
#include "resources/resourcemanager.h"
#include "net/protocol.h"

#include <iostream>
#include <guichan.hpp>
#include <physfs.h>
#include <libxml/xmlversion.h>
#include <libxml/parser.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef __USE_UNIX98
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#endif

// Account infos
int account_ID, session_ID1, session_ID2;
char sex, n_server, n_character;
SERVER_INFO *server_info;
PLAYER_INFO *char_info = new PLAYER_INFO;

Spriteset *hairset = NULL, *playerset = NULL;
Image *login_wallpaper = NULL;
Graphics *graphics;
Map *tiledMap;

char username[LEN_USERNAME];
char password[LEN_PASSWORD];
int map_address, char_ID;
short map_port;
char map_name[16];
unsigned char state;
unsigned char screen_mode;
char *dir = NULL;
char *homeDir = NULL;
int displayFlags, screenW, screenH, bitDepth;
bool useOpenGL = false;

Sound sound;
Music *bgm;

// Xml file configuration reader
Configuration config;
// Log object
Logger logger("tmw.log");
// Item database object
ItemManager itemDb;

/**
 * Listener used for responding to map start error dialog.
 */
class MapStartErrorListener : public gcn::ActionListener {
    void action(const std::string &eventId) {
        if (eventId == "ok") {
            state = LOGIN;
        }
    }
} mapStartErrorListener;

/**
 * Listener used for responding to init warning.
 */
class InitWarningListener : public gcn::ActionListener {
    void action(const std::string &eventId) {
        if (eventId == "ok") {
            state = LOGIN;
        }
    }
} initWarningListener;

/**
 * Do all initialization stuff
 */
void init_engine()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        std::cerr << "Could not initialize SDL: " <<
            SDL_GetError() << std::endl;
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    dir = new char[400];
    strcpy(dir, "");

    homeDir = new char[256];
#ifndef __USE_UNIX98
    // WIN32 and others
    strcpy(dir, "config.xml");

    // In Windows and other systems we currently store data next to executable.
    strcpy(homeDir, "");
#else
    // In UNIX we store data in ~/.manaworld/
    char *userHome;
    char *name = getlogin();
    passwd *pass;

    if (name != NULL) {
        pass = getpwnam(name);
    }
    else {
        pass = getpwuid(geteuid());
    }

    if (pass == NULL) {
        printf("Couldn't determine the user home directory. Exitting.\n");
        exit(1);
    }

    userHome = pass->pw_dir;

    // Checking if homeuser/.manaworld folder exists.
    sprintf(dir, "%s/.manaworld", userHome);
    if ((mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) &&
            (errno != EEXIST))
    {
        printf("%s can't be made, but it doesn't exist! Exitting.\n", dir);
        exit(1);
    }
    sprintf(dir, "%s/.manaworld/config.xml", userHome);

    //strcpy(homeDir, "%s/.manaworld/", userHome);
#endif

    // Checking if the configuration file exists... otherwise creates it with
    // default options !
    FILE *tmwFile = 0;
    tmwFile = fopen(dir, "r");

    // If we can't read it, it doesn't exist !
    if (tmwFile == NULL) {
        // We reopen the file in write mode and we create it
        printf("No file : %s\n, Creating Default Options...\n", dir);
        tmwFile = fopen(dir, "wt");
        if (tmwFile == NULL) {
            printf("Can't create %s file. Using Defaults.\n", dir);
        }
        else {
            fclose(tmwFile);
            // Fill configuration with defaults
            config.setValue("host", "www.lindeijer.nl");
            config.setValue("port", 6901);
            config.setValue("hwaccel", 0);
            config.setValue("opengl", 0);
            config.setValue("screen", 0);
            config.setValue("sound", 1);
            config.setValue("guialpha", 0.8f);
            config.setValue("remember", 1);
            config.setValue("sfxVolume", 100);
            config.setValue("musicVolume", 60);
            config.write(dir);
        }
    }

    config.init(dir);

    SDL_WM_SetCaption("The Mana World", NULL);

#ifdef USE_OPENGL
    useOpenGL = true;
#else
    useOpenGL = false;
#endif

    displayFlags = SDL_ANYFORMAT;

    if ((int)config.getValue("screen", 0) == 1) {
        displayFlags |= SDL_FULLSCREEN;
    }

    if (useOpenGL) {
        displayFlags |= SDL_OPENGL;
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    }
    else {
        if ((int)config.getValue("hwaccel", 0)) {
            logger.log("Attempting to use hardware acceleration.");
            displayFlags |= SDL_HWSURFACE | SDL_DOUBLEBUF;
        }
        else {
            displayFlags |= SDL_SWSURFACE;
        }
    }

    screenW = (int)config.getValue("screenwidth", 800);
    screenH = (int)config.getValue("screenheight", 600);
    bitDepth = (int)config.getValue("colordepth", 16);

    SDL_WM_SetIcon(IMG_Load("data/icons/tmw-icon.png"), NULL);

    screen = SDL_SetVideoMode(screenW, screenH, bitDepth, displayFlags);
    if (screen == NULL) {
        std::cerr << "Couldn't set " << screenW << "x" << screenH << "x" <<
            bitDepth << " video mode: " << SDL_GetError() << std::endl;
        exit(1);
    }

    char videoDriverName[64];

    if (SDL_VideoDriverName(videoDriverName, 64)) {
        logger.log("Using video driver: %s", videoDriverName);
    }
    else {
        logger.log("Using video driver: unkown");
    }

    const SDL_VideoInfo *vi = SDL_GetVideoInfo();

    logger.log("Possible to create hardware surfaces: %s",
            ((vi->hw_available) ? "yes" : "no "));
    logger.log("Window manager available: %s",
            ((vi->wm_available) ? "yes" : "no"));
    logger.log("Accelerated hardware to hardware blits: %s",
            ((vi->blit_hw) ? "yes" : "no"));
    logger.log("Accelerated hardware to hardware colorkey blits: %s",
            ((vi->blit_hw_CC) ? "yes" : "no"));
    logger.log("Accelerated hardware to hardware alpha blits: %s",
            ((vi->blit_hw_A) ? "yes" : "no"));
    logger.log("Accelerated software to hardware blits: %s",
            ((vi->blit_sw) ? "yes" : "no"));
    logger.log("Accelerated software to hardware colorkey blits: %s",
            ((vi->blit_sw_CC) ? "yes" : "no"));
    logger.log("Accelerated software to hardware alpha blits: %s",
            ((vi->blit_sw_A) ? "yes" : "no"));
    logger.log("Accelerated color fills: %s",
            ((vi->blit_fill) ? "yes" : "no"));
    logger.log("Available video memory: %d", vi->video_mem);

    //vfmt Pixel format of the video device

    // Create the graphics context
    graphics = new Graphics();

    ResourceManager *resman = ResourceManager::getInstance();

    login_wallpaper = resman->getImage(
            "graphics/images/login_wallpaper.png");
    Image *playerImg = resman->getImage(
            "graphics/sprites/player_male_base.png");
    Image *hairImg = resman->getImage(
            "graphics/sprites/player_male_hair.png");

    if (!login_wallpaper) logger.error("Couldn't load login_wallpaper.png");
    if (!playerImg) logger.error("Couldn't load player_male_base.png");
    if (!hairImg) logger.error("Couldn't load player_male_hair.png");

    playerset = new Spriteset(playerImg, 64, 120);
    hairset = new Spriteset(hairImg, 40, 40);

    gui = new Gui(graphics);
    state = LOGIN;

    // Initialize sound engine
    try {
        if (config.getValue("sound", 0) == 1) {
            sound.init();
        }
        sound.setSfxVolume((int)config.getValue("sfxVolume", 100));
        sound.setMusicVolume((int)config.getValue("musicVolume", 60));
    }
    catch (const char *err) {
        state = ERROR;
        new OkDialog("Sound Engine", err, &initWarningListener);
        logger.log("Warning: %s", err);
    }
}

/** Clear the engine */
void exit_engine()
{
    config.write(dir);
    delete[] dir;
    delete gui;
    delete graphics;

    // Shutdown libxml
    xmlCleanupParser();

    ResourceManager::deleteInstance();
}

/** Main */
int main(int argc, char *argv[])
{
    // Initialize libxml2 and check for potential ABI mismatches between
    // compiled version and the shared library actually used.
    xmlInitParser();
    LIBXML_TEST_VERSION;

    // Initialize PhysicsFS
    PHYSFS_init(argv[0]);

    init_engine();

    SDL_Event event;

    while (state != EXIT)
    {
        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    state = EXIT;
                    break;
            }

            guiInput->pushInput(event);
        }

        ResourceManager *resman = ResourceManager::getInstance();

        switch (state) {
            case LOGIN:
                logger.log("State: LOGIN");
                sound.playMusic("data/music/Ivano(de)Jeanette.ogg");
                /*bgm = resman->getMusic("music/Ivano(de)Jeanette.ogg");
                bgm->play(-1);*/
                login();
                break;
            case CHAR_SERVER:
                logger.log("State: CHAR_SERVER");
                char_server();
                break;
            case CHAR_SELECT:
                logger.log("State: CHAR_SELECT");
                charSelect();
                break;
            case GAME:
                sound.fadeOutMusic(1000);
                //bgm->stop();
                logger.log("State: GAME");
                try {
                    map_start();
                    game();
                }
                catch (const char* err) {
                    state = ERROR;
                    new OkDialog("Error", err, &mapStartErrorListener);
                }
                break;
            case ERROR:
                // Redraw GUI
                login_wallpaper->draw(screen, 0, 0);
                gui->logic();
                gui->draw();
                graphics->updateScreen();
                break;
            default:
                state = EXIT;
                break;
        }
    }
    logger.log("State: EXIT");
    exit_engine();
    PHYSFS_deinit();
    return 0;
}

// GetSkill Function
// Retrieves the level of the skill for the ID value given.
// This function also increases the XP of the skill by the given parameter.
// Call n_base to return the actual value, regardless of equipment modifiers.
// ---by Kyokai
/*
int PLAYER_INFO::GetSkill(int n_ID, int n_XP, int n_base)
{
    if (n_ID > N_SKILLS || n_ID < 0) // out of cheese error, abort function
        return 0;
    // 1. raise the exp value
    m_Skill[n_ID].exp += (short)(n_XP * m_Skill[n_ID].mod);

    // 2. Check for level up
    if (m_Skill[n_ID].exp >= 20 * ((m_Skill[n_ID].level)^(6/5)))
    {
        m_Skill[n_ID].level += 1;
        m_Skill[n_ID].exp = 0;
        // TO DO: send the user a message that tells him his
        // skill just leveled up!
    }

    // 3. getting the return value
    int r = m_Skill[n_ID].level;
    if (n_base)
    {
        // TO DO: alter values based on equipment bonuses
    }

    return r; // return the value
}
*/
