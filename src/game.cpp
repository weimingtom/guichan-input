/*
 *  The Mana World
 *  Copyright (C) 2004  The Mana World Development Team
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

#include "beingmanager.h"
#include "channelmanager.h"
#include "commandhandler.h"
#include "configuration.h"
#include "effectmanager.h"
#include "emoteshortcut.h"
#include "engine.h"
#include "flooritemmanager.h"
#include "game.h"
#include "graphics.h"
#include "itemshortcut.h"
#include "joystick.h"
#include "keyboardconfig.h"
#include "localplayer.h"
#include "log.h"
#include "main.h"
#include "map.h"
#include "npc.h"
#include "particle.h"
#include "playerrelations.h"
#include "sound.h"

#include "gui/widgets/chattab.h"
#include "gui/buy.h"
#include "gui/buysell.h"
#include "gui/chat.h"
#include "gui/confirmdialog.h"
#include "gui/debugwindow.h"
#include "gui/emoteshortcutcontainer.h"
#include "gui/equipmentwindow.h"
#include "gui/gui.h"
#include "gui/help.h"
#include "gui/inventorywindow.h"
#include "gui/shortcutwindow.h"
#include "gui/itemshortcutcontainer.h"
#include "gui/minimap.h"
#include "gui/ministatus.h"
#include "gui/npcdialog.h"
#include "gui/okdialog.h"
#include "gui/outfitwindow.h"
#include "gui/sdlinput.h"
#include "gui/sell.h"
#include "gui/setup.h"
#include "gui/skilldialog.h"
#include "gui/statuswindow.h"
#include "gui/trade.h"
#include "gui/viewport.h"
#include "gui/windowmenu.h"
#include "gui/partywindow.h"
#include "gui/guildwindow.h"
#include "gui/npcpostdialog.h"
#include "gui/quitdialog.h"
#include "gui/specialswindow.h"
#include "gui/storagewindow.h"

#include "net/gamehandler.h"
#include "net/generalhandler.h"
#include "net/net.h"

#include "resources/imagewriter.h"

#include "utils/gettext.h"

#include <guichan/exception.hpp>
#include <guichan/focushandler.hpp>

#include <fstream>
#include <sstream>
#include <string>

std::string map_path;

volatile int tick_time;
volatile int fps = 0, frame = 0;

Engine *engine = NULL;
Joystick *joystick = NULL;

extern Window *weightNotice;
extern Window *deathNotice;
QuitDialog *quitDialog = NULL;
OkDialog *disconnectedDialog = NULL;

ChatWindow *chatWindow;
StatusWindow *statusWindow;
MiniStatusWindow *miniStatusWindow;
BuyDialog *buyDialog;
SellDialog *sellDialog;
BuySellDialog *buySellDialog;
InventoryWindow *inventoryWindow;
SkillDialog *skillDialog;
PartyWindow *partyWindow;
GuildWindow *guildWindow;
NpcDialog *npcDialog;
NpcPostDialog *npcPostDialog;
StorageWindow *storageWindow;
Minimap *minimap;
EquipmentWindow *equipmentWindow;
TradeWindow *tradeWindow;
HelpWindow *helpWindow;
DebugWindow *debugWindow;
ShortcutWindow *itemShortcutWindow;
ShortcutWindow *emoteShortcutWindow;
OutfitWindow *outfitWindow;
SpecialsWindow *specialsWindow;

BeingManager *beingManager = NULL;
FloorItemManager *floorItemManager = NULL;
ChannelManager *channelManager = NULL;
CommandHandler *commandHandler = NULL;
Particle *particleEngine = NULL;
EffectManager *effectManager = NULL;

ChatTab *localChatTab = NULL;

/**
 * Tells the max tick value,
 * setting it back to zero (and start again).
 */
const int MAX_TICK_VALUE = 10000;
/**
 * Set the milliseconds value of a tick time.
 */
const int MILLISECONDS_IN_A_TICK = 10;

/**
 * Listener used for exiting handling.
 */
namespace {
    class ExitListener : public gcn::ActionListener
    {
    public:
        void action(const gcn::ActionEvent &event)
        {
            if (event.getId() == "yes" || event.getId() == "ok")
                state = STATE_EXIT;

            disconnectedDialog = NULL;
        }
    } exitListener;
}

/**
 * Advances game logic counter.
 * Called every 10 milliseconds by SDL_AddTimer()
 * @see MILLISECONDS_IN_A_TICK value
 */
Uint32 nextTick(Uint32 interval, void *param)
{
    tick_time++;
    if (tick_time == MAX_TICK_VALUE) tick_time = 0;
    return interval;
}

/**
 * Updates fps.
 * Called every seconds by SDL_AddTimer()
 */
Uint32 nextSecond(Uint32 interval, void *param)
{
    fps = frame;
    frame = 0;

    return interval;
}

/**
 * @return the elapsed time in milliseconds
 * between two tick values.
 */
int get_elapsed_time(int start_time)
{
    if (start_time <= tick_time)
        return (tick_time - start_time) * MILLISECONDS_IN_A_TICK;
    else
        return (tick_time + (MAX_TICK_VALUE - start_time))
                * MILLISECONDS_IN_A_TICK;
}

/**
 * Initialize every game sub-engines in the right order
 */
static void initEngines()
{
    engine = new Engine;

    beingManager = new BeingManager;
    commandHandler = new CommandHandler;
    floorItemManager = new FloorItemManager;
    channelManager = new ChannelManager;
    effectManager = new EffectManager;

    particleEngine = new Particle(NULL);
    particleEngine->setupEngine();
}

/**
 * Create all the various globally accessible gui windows
 */
static void createGuiWindows()
{
    setupWindow->clearWindowsForReset();

    // Create dialogs
    chatWindow = new ChatWindow;
    buyDialog = new BuyDialog;
    sellDialog = new SellDialog;
    tradeWindow = new TradeWindow;
    partyWindow = new PartyWindow;
    guildWindow = new GuildWindow;
    buySellDialog = new BuySellDialog;
    equipmentWindow = new EquipmentWindow(player_node->mEquipment.get());
    npcDialog = new NpcDialog;
    npcPostDialog = new NpcPostDialog;
    storageWindow = new StorageWindow;
    statusWindow = new StatusWindow;
    miniStatusWindow = new MiniStatusWindow;
    inventoryWindow = new InventoryWindow;
    skillDialog = new SkillDialog;
    minimap = new Minimap;
    helpWindow = new HelpWindow;
    debugWindow = new DebugWindow;
    itemShortcutWindow = new ShortcutWindow("ItemShortcut",
                                            new ItemShortcutContainer);
    emoteShortcutWindow = new ShortcutWindow("EmoteShortcut",
                                             new EmoteShortcutContainer);
    outfitWindow = new OutfitWindow();
    specialsWindow = new SpecialsWindow();

    localChatTab = new ChatTab(_("General"));

    if (config.getValue("logToChat", 0))
    {
        logger->setChatWindow(chatWindow);
    }

    Net::getGeneralHandler()->guiWindowsLoaded();
}

#define del_0(X) { delete X; X = 0; }

/**
 * Destroy all the globally accessible gui windows
 */
static void destroyGuiWindows()
{
    Net::getGeneralHandler()->guiWindowsUnloaded();
    logger->setChatWindow(NULL);
    del_0(localChatTab); // Need to do this first, so it can remove itself
    del_0(chatWindow);
    del_0(statusWindow);
    del_0(miniStatusWindow);
    del_0(buyDialog);
    del_0(sellDialog);
    del_0(buySellDialog);
    del_0(inventoryWindow);
    del_0(partyWindow);
    del_0(npcDialog);
    del_0(npcPostDialog);
    del_0(guildWindow);
    del_0(skillDialog);
    del_0(minimap);
    del_0(equipmentWindow);
    del_0(tradeWindow);
    del_0(helpWindow);
    del_0(debugWindow);
    del_0(itemShortcutWindow);
    del_0(emoteShortcutWindow);
    del_0(storageWindow);
    del_0(outfitWindow);
    del_0(specialsWindow);
}

Game::Game():
    mLastTarget(Being::UNKNOWN),
    mLogicCounterId(0), mSecondsCounterId(0)
{
    createGuiWindows();

    mWindowMenu = new WindowMenu;
    windowContainer->add(mWindowMenu);

    initEngines();

    // Initialize logic and seconds counters
    tick_time = 0;
    mLogicCounterId = SDL_AddTimer(MILLISECONDS_IN_A_TICK, nextTick, NULL);
    mSecondsCounterId = SDL_AddTimer(1000, nextSecond, NULL);

    // This part is eAthena specific
    // For Manaserv, the map is obtained
    // with the GPMSG_PLAYER_MAP_CHANGE flag.
    map_path = map_path.substr(0, map_path.rfind("."));
    if (!map_path.empty())
        engine->changeMap(map_path);

    // Initialize beings
    beingManager->setPlayer(player_node);

    /*
     * To prevent the server from sending data before the client
     * has initialized, I've modified it to wait for a "ping"
     * from the client to complete its initialization
     *
     * Note: This only affects the latest eAthena version.  This
     * packet is handled by the older version, but its response
     * is ignored by the client
     */
    Net::getGameHandler()->ping(tick_time);

    // Initialize frame limiting
    config.addListener("fpslimit", this);
    optionChanged("fpslimit");

    Joystick::init();
    // TODO: The user should be able to choose which one to use
    // Open the first device
    if (Joystick::getNumberOfJoysticks() > 0)
        joystick = new Joystick(0);

    setupWindow->setInGame(true);
}

Game::~Game()
{
    delete mWindowMenu;

    destroyGuiWindows();

    delete beingManager;
    delete player_node;
    delete floorItemManager;
    delete channelManager;
    delete commandHandler;
    delete joystick;
    delete particleEngine;
    delete engine;

    viewport->setMap(NULL);
    player_node = NULL;
    beingManager = NULL;
    floorItemManager = NULL;
    joystick = NULL;

    SDL_RemoveTimer(mLogicCounterId);
    SDL_RemoveTimer(mSecondsCounterId);
}

bool Game::saveScreenshot()
{
    static unsigned int screenshotCount = 0;

    SDL_Surface *screenshot = graphics->getScreenshot();

    // Search for an unused screenshot name
    std::stringstream filenameSuffix;
    std::stringstream filename;
    std::fstream testExists;
    bool found = false;

    do {
        screenshotCount++;
        filenameSuffix.str("");
        filename.str("");
#if (defined __USE_UNIX98 || defined __FreeBSD__)
        filename << getHomeDirectory() << "/";
#elif defined __APPLE__
        filename << PHYSFS_getUserDir();
        filename << "Desktop/";
#endif
        filenameSuffix << "Mana_Screenshot_" << screenshotCount << ".png";
        filename << filenameSuffix.str();
        testExists.open(filename.str().c_str(), std::ios::in);
        found = !testExists.is_open();
        testExists.close();
    } while (!found);

    const bool success = ImageWriter::writePNG(screenshot, filename.str());

    if (success)
    {
        std::stringstream chatlogentry;
        // TODO: Make it one complete gettext string below
        chatlogentry << _("Screenshot saved as ") << filenameSuffix.str();
        localChatTab->chatLog(chatlogentry.str(), BY_SERVER);
    }
    else
    {
        localChatTab->chatLog(_("Saving screenshot failed!"), BY_SERVER);
        logger->log("Error: could not save screenshot.");
    }

    SDL_FreeSurface(screenshot);

    return success;
}

void Game::optionChanged(const std::string &name)
{
    int fpsLimit = (int) config.getValue("fpslimit", 60);

    mMinFrameTime = fpsLimit ? 1000 / fpsLimit : 0;

    // Reset draw time to current time
    mDrawTime = tick_time * MILLISECONDS_IN_A_TICK;
}

void Game::logic()
{
    // mDrawTime has a higher granularity than gameTime in order to be able to
    // work with minimum frame durations in milliseconds.
    int gameTime = tick_time;
    mDrawTime = tick_time * MILLISECONDS_IN_A_TICK;
    SDL_Event event;

    while (state == STATE_GAME)
    {
        if (Map *map = engine->getCurrentMap())
            map->update(get_elapsed_time(gameTime));

        gui->focusTop(false);

        if (joystick)
            joystick->update();

        // Events
        while (SDL_PollEvent(&event))
        {
            // Quit event
            if (event.type == SDL_QUIT)
            {
                state = STATE_EXIT;

                // We can safely skip everything else in here
                return;
            }
            else if (event.type == SDL_KEYUP)
            {
                // Make sure guichan can recognize character keys
                if (!event.key.keysym.unicode)
                    event.key.keysym.unicode = event.key.keysym.sym;
            }

            gui->focusTop();
            guiInput->pushInput(event);
        }

        keyboard.processStates();


        // Handle all necessary game logic
        while (get_elapsed_time(gameTime) > 0)
        {
            beingManager->logic();
            particleEngine->update();
            gameTime++;
        }

        gui->logic();

        // This is done because at some point tick_time will wrap.
        gameTime = tick_time;

        // Update the screen when application is active, delay otherwise.
        if (SDL_GetAppState() & SDL_APPACTIVE)
        {
            // Draw a frame if either frames are not limited or enough time has
            // passed since the last frame.
            if (!mMinFrameTime ||
                get_elapsed_time(mDrawTime / 10) > mMinFrameTime)
            {
                frame++;
                gui->draw();
                graphics->updateScreen();
                mDrawTime += mMinFrameTime;

                // Make sure to wrap mDrawTime, since tick_time will wrap.
                if (mDrawTime > MAX_TICK_VALUE * MILLISECONDS_IN_A_TICK)
                    mDrawTime -= MAX_TICK_VALUE * MILLISECONDS_IN_A_TICK;
            }
            else
            {
                SDL_Delay(MILLISECONDS_IN_A_TICK);
            }
        }
        else
        {
            SDL_Delay(MILLISECONDS_IN_A_TICK);
            mDrawTime = tick_time * MILLISECONDS_IN_A_TICK;
        }

        // Handle network stuff
        Net::getGeneralHandler()->flushNetwork();
        if (!Net::getGameHandler()->isConnected())
        {
            if (state != STATE_ERROR)
            {
                errorMessage = _("The connection to the server was lost, "
                                 "the program will now quit");
            }

            if (!disconnectedDialog)
            {
                disconnectedDialog = new OkDialog(_("Network Error"), errorMessage);
                disconnectedDialog->addActionListener(&exitListener);
                disconnectedDialog->requestMoveToTop();
            }
        }
    }
}

void Game::quit()
{
    if (!quitDialog)
    {
        quitDialog = new QuitDialog(&quitDialog);
        quitDialog->requestMoveToTop();
    }
    else
    {
        quitDialog->action(gcn::ActionEvent(NULL, "cancel"));
    }
}
