//==============================================================================
// Project:   Wii-Tac-Toe
//==============================================================================

/**
 * @mainpage Wii-Tac-Toe Documentation
 * Wii-Tac-Toe is a Tic-Tac-Toe game for the Nintendo Wii.
 */
//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include <cstdlib>
#include <wiiuse/wpad.h>
#include "grrlib_class.h"
#include "game.h"

#define SYS_NOTSET          -1
#define SYS_RETURNTOHBMENU   7

/**
 * Hardware button state.
 */
s32 HWButton = SYS_NOTSET;

/**
 * Callback for the reset button on the Wii.
 */
void WiiResetPressed([[maybe_unused]] u32 irq, [[maybe_unused]] void* ctx)
{
    HWButton = SYS_RETURNTOMENU;
}

/**
 * Callback for the power button on the Wii.
 */
void WiiPowerPressed()
{
    HWButton = SYS_POWEROFF_STANDBY;
}

/**
 * Callback for the power button on the Wiimote.
 * @param[in] chan The Wiimote that pressed the button.
 */
void WiimotePowerPressed([[maybe_unused]] s32 chan)
{
    HWButton = SYS_POWEROFF_STANDBY;
    //SYS_POWEROFF
}

/**
 * Entry point.
 * @param[in] argc The number of arguments invoked with the program.
 * @param[in] argv The array containing the arguments.
 * @return 0 on clean exit, an error code otherwise.
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    // Video initialization
    Initialize();

    // Wiimote initialization
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, Screen::GetWidth(), Screen::GetHeight());

    // Game initialization
    Game *MyGame = new Game(Screen::GetWidth(), Screen::GetHeight());

    SYS_SetResetCallback(WiiResetPressed);
    SYS_SetPowerCallback(WiiPowerPressed);
    WPAD_SetPowerButtonCallback(WiimotePowerPressed);

    while(true)
    {
        MyGame->Paint();

        WPAD_ScanPads();
        if(MyGame->ControllerManager() == true)
        {
            break;
        }
        if(HWButton != SYS_NOTSET)
        {
            break;
        }

        Render();
    }

    delete MyGame;
    WPAD_Shutdown();
    Exit();

    if(HWButton != SYS_NOTSET && HWButton != SYS_RETURNTOHBMENU)
    {
        SYS_ResetSystem(HWButton, 0, 0);
    }

    return 0;
}
