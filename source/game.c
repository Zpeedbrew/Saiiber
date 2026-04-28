
#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <wiiuse/wpad.h>


#include "Letter_Gothic_Std_14_Bold_png.h"

int main() {
    float l1=0, l2=0;
    float a=0;
    int camZ=13.0f;

    GRRLIB_Init();
    WPAD_Init();


    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(Letter_Gothic_Std_14_Bold_png);
    GRRLIB_InitTileSet(tex_font, 11, 24, 32);


    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);

    while(1) {
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_PLUS) camZ++;
        if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_MINUS) camZ--;

        GRRLIB_Camera3dSettings(0.0f,0.0f,camZ, 0,1,0, 0,0,0);
        GRRLIB_SetLightAmbient(0x333333FF);

        GRRLIB_3dMode(0.1, 1000, 45, 0, 1);

            // Set all light off to get the sphere no light sourced (only get the vertex color)
            GRRLIB_SetLightOff();
            GRRLIB_ObjectView(sin(l1)*4.0f,0.0f,cos(l1)*4.0f, 0,0,0,1,1,1);
            GRRLIB_DrawSphere(0.2f, 20, 20, true, 0xFF0000FF);
        

        
            // Set all light off to get the sphere no light sourced (only get the vertex color)
            GRRLIB_SetLightOff();
            GRRLIB_ObjectView(0.0f,sin(l2)*4.0f,cos(l2)*4.0f, 0,0,0,1,1,1);
            GRRLIB_DrawSphere(0.2f, 20, 20, true, 0x00FF00FF);
        

        // Set a dummy black light to get the ambient one when no light is selected
        GRRLIB_SetLightDiff(0,(guVector){0.0f,0.0f,0.0f},20.0f,1.0f,0x000000FF);

        
            GRRLIB_SetLightDiff(0,(guVector){sin(l1)*4.0f,0.0f,cos(l1)*4.0f},20.0f,1.0f,0xFF0000FF);
            l1+=0.03f;
        

            GRRLIB_SetLightDiff(1,(guVector){0.0f,sin(l2)*4.0f,cos(l2)*4.0f},20.0f,1.0f,0x00FF00FF);
            l2+=0.05f;

        GRRLIB_ObjectView(0,0,0, a,a*2,a*3,1,1,1);
        

        a+=0.5f;

        // Switch to 2D Mode to display text
        GRRLIB_2dMode();
        //title text 
        GRRLIB_Printf((640-(21*21))/2, 20, tex_font, 0xFF0000FF, 5, "Saii");
        GRRLIB_Printf((640-(3*3))/2, 20, tex_font, 0x0000FFFF, 5, "ber");

        GRRLIB_Printf((640-(16*14))/2, 400, tex_font, 0xFF0000FF, 2, "exit");
        //need to button stuff eventuly.
        /*
        exit(0)
        */
        GRRLIB_Printf((640-(16*14))/2, 100, tex_font, 0xFF0000FF, 2, "play");
        GRRLIB_Printf((640-(16*14))/2, 150, tex_font, 0xFF0000FF, 2, "settings");
        GRRLIB_Printf((640-(16*14))/2, 200, tex_font, 0xFF0000FF, 2, "debug");
        GRRLIB_Printf((640-(16*14))/2, 250, tex_font, 0xFF0000FF, 2, "settings");
        GRRLIB_Printf((640-(16*14))/2, 300, tex_font, 0xFF0000FF, 2, "quick play");
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "credits");
        //debug text neeed to make togglable
        /*
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "current version:1.6");
        GRRLIB_Printf((640-(16*14))/2, 350, tex_font, 0xFF0000FF, 2, "rendering engine:GRRLIB");
        */



        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}



#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <charconv>
#include <limits>
#include <format>
#include <wiiuse/wpad.h>
#include <ogc/conf.h>
#include <ogc/lwp_watchdog.h>
#include "grrlib.h"
#include "grrlib_class.h"
#include "tools.h"
#include "grid.h"
#include "audio.h"
#include "button.h"
#include "cursor.h"
#include "player.h"
#include "language.h"
#include "types.h"
#include "game.h"

// Graphics
#include "splash_png.h"
#include "splash_arm_png.h"
#include "backg_png.h"
#include "backg_hover_png.h"
#include "hover_png.h"

// Font
#include "../fonts/Swis721_Ex_BT.h"

/**
 * Array to hold the position of each zone.
 */
static constexpr Point Table[3][3] = {
    {Point(180, 28), Point(180, 131), Point(180, 233)},
    {Point(322, 28), Point(322, 131), Point(322, 233)},
    {Point(464, 28), Point(464, 131), Point(464, 233)}};

/**
 * Maximum digits + null terminator.
 */
static constexpr size_t MaxScoreLength = std::numeric_limits<u16>::digits10 + 2;

/**
 * Constructor for the Game class.
 * @param[in] GameScreenWidth Screen width.
 * @param[in] GameScreenHeight Screen height.
 */
Game::Game(u16 GameScreenWidth, u16 GameScreenHeight) :
    ScreenWidth(GameScreenWidth),
    ScreenHeight(GameScreenHeight)
{
    std::srand(std::time(nullptr));

    GameGrid = new Grid();
    Lang = new Language();

    DefaultFont = GRRLIB_LoadTTF(Swis721_Ex_BT, Swis721_Ex_BT_size);

    for(u8 x = 0; x < 3; ++x)
    {
        for(u8 y = 0; y < 3; ++y)
        {
            GridSign[x][y].SetLocation(Table[x][y]);
        }
    }

    Hand[0].SetVisible(false);
    Hand[1].SetVisible(false);
    Hand[2].SetVisible(false);
    Hand[3].SetVisible(false);

    ExitButton[0] = new Button(buttonType::Home);
    ExitButton[0]->SetFont(DefaultFont);
    ExitButton[0]->SetLeft(430);
    ExitButton[0]->SetTop(20);
    ExitButton[0]->SetTextHeight(20);
    ExitButton[0]->SetCaption(Lang->String("Close"));

    ExitButton[1] = new Button(buttonType::HomeMenu);
    ExitButton[1]->SetFont(DefaultFont);
    ExitButton[1]->SetLeft((ScreenWidth / 2.0f) + 20.0f);
    ExitButton[1]->SetTop(165);
    ExitButton[1]->SetCaption(Lang->String("Reset"));

    ExitButton[2] = new Button(buttonType::HomeMenu);
    ExitButton[2]->SetFont(DefaultFont);
    ExitButton[2]->SetLeft((ScreenWidth / 2.0f) - ExitButton[1]->GetWidth() - 20.0f);
    ExitButton[2]->SetTop(165);
    ExitButton[2]->SetCaption(Lang->String("Return to Loader"));

    MenuButton[0] = new Button(buttonType::StdMenu);
    MenuButton[0]->SetFont(DefaultFont);
    MenuButton[0]->SetLeft((ScreenWidth / 2.0f) - (MenuButton[0]->GetWidth() / 2.0f));
    MenuButton[0]->SetTop(92);
    MenuButton[0]->SetCaption(Lang->String("2 Players (1 Wiimote)"));

    MenuButton[1] = new Button(buttonType::StdMenu);
    MenuButton[1]->SetFont(DefaultFont);
    MenuButton[1]->SetLeft((ScreenWidth / 2.0f) - (MenuButton[1]->GetWidth() / 2.0f));
    MenuButton[1]->SetTop(292);
    MenuButton[1]->SetCaption(Lang->String("1 Player (Vs AI)"));

    MenuButton[2] = new Button(buttonType::StdMenu);
    MenuButton[2]->SetFont(DefaultFont);
    MenuButton[2]->SetLeft((ScreenWidth / 2.0f) - (MenuButton[2]->GetWidth() / 2.0f));
    MenuButton[2]->SetTop(192);
    MenuButton[2]->SetCaption(Lang->String("2 Players (2 Wiimote)"));

    WTTPlayer[0].SetSign('X');
    WTTPlayer[0].SetName(Lang->String("PLAYER 1"));
    WTTPlayer[1].SetSign('O');
    WTTPlayer[1].SetName(Lang->String("PLAYER 2"));

    GameImg = Texture::CreateFromPNG(backg_png);
    GameHoverImg = Texture::CreateFromPNG(backg_hover_png);
    SplashImg = Texture::CreateFromPNG(splash_png);
    SplashArmImg = Texture::CreateFromPNG(splash_arm_png);
    HoverImg = Texture::CreateFromPNG(hover_png);
    CopiedImg = std::make_unique<Texture>(ScreenWidth, ScreenHeight);
    GameText = std::make_unique<Texture>(ScreenWidth, ScreenHeight);

    // Player name with a shadow offset of -2, 2 (includes game background)
    // GameText should only be modified when player names changed.
    GameImg->Draw(0, 0);
    PrintWrapText(44, 48, 125, WTTPlayer[0].GetName(), 15, 0xFFFFFFFF, 0x6BB6DEFF, -2, 2);
    PrintWrapText(44, 143, 125, WTTPlayer[1].GetName(), 15, 0xFFFFFFFF, 0xE6313AFF, -2, 2);
    PrintWrapText(44, 248, 125, Lang->String("TIE GAME"), 15, 0xFFFFFFFF, 0x109642FF, -2, 2);
    GameText->CopyScreen(0, 0, true);

    // Build Start Screen background
    SplashImg->Draw(0, 0);
    GRRLIB_PrintfTTF(50, 310, DefaultFont,
        std::format(std::runtime_format(Lang->String("Programmer: {}")), "GUINEA7PIG").c_str(),
        11, 0xFFFFFFFF);
    GRRLIB_PrintfTTF(50, 330, DefaultFont,
        std::format(std::runtime_format(Lang->String("Graphics: {}")), "Mr_Nick666").c_str(),
        11, 0xFFFFFFFF);
    text = Lang->String("Press The A Button");
    GRRLIB_PrintfTTF((ScreenWidth / 2) - (GRRLIB_WidthTTF(DefaultFont, text.c_str(), 20) / 2),
                    400, DefaultFont, text.c_str(), 20, 0x000000FF);
    SplashImg->CopyScreen(0, 0, true);

    // Set handle for arm rotation
    SplashArmImg->SetHandle(8, 70);

    GameAudio = std::make_unique<Audio>();

    RUMBLE_Init();
    NewGame();
}

/**
 * Destructor for the Game class.
 */
Game::~Game()
{
    GRRLIB_FreeTTF(DefaultFont);

    delete GameGrid;
    delete Lang;

    for(u8 i = 0; i < ExitButton.size(); ++i)
    {
        delete ExitButton[i];
    }

    for(u8 i = 0; i < MenuButton.size(); ++i)
    {
        delete MenuButton[i];
    }
}

/**
 * Draw the proper screen.
 */
void Game::Paint()
{
    switch(CurrentScreen)
    {
        case gameScreen::Start:
            StartScreen();
            break;
        case gameScreen::Menu:
            MenuScreen(true);
            break;
        case gameScreen::Home:
            ExitScreen();
            break;
        case gameScreen::Game:
            GameScreen(true);
            // AI
            if(RoundFinished == false && WTTPlayer[CurrentPlayer].GetType() == playerType::CPU)
            {   // AI
                if(AIThinkLoop > (rand() % 10 + 20))
                {
                    GameGrid->SetPlayerAI(WTTPlayer[CurrentPlayer].GetSign());
                    TurnIsOver();
                    AIThinkLoop = 0;
                }
                else
                {
                    ++AIThinkLoop;
                }
            }
            break;
        default:
            GRRLIB_FillScreen(0x000000FF);
    }

    if(CurrentScreen != gameScreen::Start &&
        WPAD_Probe(WPAD_CHAN_0, nullptr) == WPAD_ERR_NO_CONTROLLER)
    {   // Controller is disconnected
        Rectangle(0, 0, ScreenWidth, ScreenHeight, 0x000000B2, 1);
    }
    else
    {
        Hand[3].Paint();
        Hand[2].Paint();
        Hand[1].Paint();
        Hand[0].Paint();
    }
    if(ShowFPS == true)
    {
        CalculateFrameRate();
        const auto strFPS = std::format("FPS: {}", FPS);
        GRRLIB_PrintfTTF(14, 444, DefaultFont, strFPS.c_str(), 17, 0xFFFFFFFF);
        GRRLIB_PrintfTTF(16, 446, DefaultFont, strFPS.c_str(), 17, 0x808080FF);
        GRRLIB_PrintfTTF(15, 445, DefaultFont, strFPS.c_str(), 17, 0x000000FF);
    }
}

/**
 * Draw the start screen.
 */
void Game::StartScreen()
{
    static f32 ArmRotation = 0;
    static bool ArmDirection = 0;

    SplashImg->Draw(0, 0); // Background

    if(ArmDirection == 0)
    {
        ArmRotation += 0.5f;
        if(ArmRotation > 40.0f)
        {
            ArmDirection = 1;
        }
    }
    else
    {
        ArmRotation -= 0.5f;
        if(ArmRotation < -15.0f)
        {
            ArmDirection = 0;
        }
    }
    GRRLIB_ClipDrawing(158, 40, 100, 145);
    SplashArmImg->Draw(146, 62, ArmRotation); // Arm
    GRRLIB_ClipReset();
}

/**
 * Draw the game screen.
 * @param[in] CopyScreen If true the screen is copied to a buffer.
 */
void Game::GameScreen(bool CopyScreen)
{
    if(Copied == false)
    {   // Copy static element
        GameText->Draw(0, 0); // Background image with some text

        // Draw score with a shadow offset of -2, 2
        char ScoreText[MaxScoreLength];
        if(auto [ptr, ec] = std::to_chars(ScoreText, ScoreText + MaxScoreLength, WTTPlayer[0].GetScore()); ec == std::errc{})
        {
            *ptr = '\0';
            const auto TextLeft = 104 - GRRLIB_WidthTTF(DefaultFont, &ScoreText[0], 35) / 2;
            GRRLIB_PrintfTTF(TextLeft, 77, DefaultFont, &ScoreText[0], 35, 0x6BB6DEFF);
            GRRLIB_PrintfTTF(TextLeft - 2, 75, DefaultFont, &ScoreText[0], 35, 0xFFFFFFFF);
        }
        if(auto [ptr, ec] = std::to_chars(ScoreText, ScoreText + MaxScoreLength, WTTPlayer[1].GetScore()); ec == std::errc{})
        {
            *ptr = '\0';
            const auto TextLeft = 104 - GRRLIB_WidthTTF(DefaultFont, &ScoreText[0], 35) / 2;
            GRRLIB_PrintfTTF(TextLeft, 177, DefaultFont, &ScoreText[0], 35, 0xE6313AFF);
            GRRLIB_PrintfTTF(TextLeft - 2, 175, DefaultFont, &ScoreText[0], 35, 0xFFFFFFFF);
        }
        if(auto [ptr, ec] = std::to_chars(ScoreText, ScoreText + MaxScoreLength, TieGame); ec == std::errc{})
        {
            *ptr = '\0';
            const auto TextLeft = 104 - GRRLIB_WidthTTF(DefaultFont, &ScoreText[0], 35) / 2;
            GRRLIB_PrintfTTF(TextLeft, 282, DefaultFont, &ScoreText[0], 35, 0x109642FF);
            GRRLIB_PrintfTTF(TextLeft - 2, 280, DefaultFont, &ScoreText[0], 35, 0xFFFFFFFF);
        }

        // Draw text at the bottom with a shadow offset of 1, 1
        PrintWrapText(130, 420, 390, text, 15, 0x8C8A8CFF, 0x111111FF, 1, 1);

        if(CopyScreen == true)
        {
            CopiedImg->CopyScreen();
            Copied = true;
        }
    }
    if(CopyScreen == true)
    {
        CopiedImg->Draw(0, 0);
    }

    const u32 HoverColor = (WTTPlayer[CurrentPlayer].GetSign() == 'X') ? 0x0093DDFF : 0xDA251DFF;

    // Draw grid content
    if(RoundFinished == true)
    {
        if(AlphaDirection == true)
        {
            SymbolAlpha += 2;
            if(SymbolAlpha > 128)
            {
                AlphaDirection = !AlphaDirection;
            }
        }
        else
        {
            SymbolAlpha -= 2;
            if(SymbolAlpha < 5)
            {
                AlphaDirection = !AlphaDirection;
            }
        }
    }
    for(u8 x = 0; x < 3; ++x)
    {
        for(u8 y = 0; y < 3; ++y)
        {
            GridSign[x][y].SetPlayer(GameGrid->GetPlayerAtPos(x, y));
            GridSign[x][y].SetColor(0xFFFFFFFF);
            GridSign[x][y].Paint();
            if(GameGrid->WinningBoard[x][y] == true)
            {
                GridSign[x][y].SetColor(HoverColor);
                GridSign[x][y].SetAlpha(SymbolAlpha);
                GridSign[x][y].Paint();
            }
        }
    }

    if(SelectZone() == true)
    {
        // Draw selection box
        if(GameGrid->GetPlayerAtPos(HandX, HandY) == ' ')
        {
            HoverImg->Draw(Table[HandX][HandY].GetX(), Table[HandX][HandY].GetY(),
                0, 1, 1, HoverColor);
        }
    }
    else
    {
        // 40 = radius
        // 52 = half of image size
        if(PtInCircle(65, 409, 40, Hand[0].GetLeft(), Hand[0].GetTop()) == true)
        {
            GameHoverImg->Draw(65-52, 409.5-52, 0, 1, 1, 0xFFFFFFFF);
            ButtonOn(0);
            FocusedButton = 0;
        }
        else if(PtInCircle(571, 409, 40, Hand[0].GetLeft(), Hand[0].GetTop()) == true)
        {
            GameHoverImg->Draw(571-52, 409.5-52, 0, 1, 1, 0xFFFFFFFF);
            ButtonOn(1);
            FocusedButton = 1;
        }
        else
        {
            FocusedButton = -1;
        }
    }
}

/**
 * Draw the HOME screen.
 */
void Game::ExitScreen()
{
    GameAudio->PauseMusic(true);
    if(Copied == false)
    {   // Copy static element
        switch(LastScreen)
        {
            case gameScreen::Game:
                GameScreen(false);
                break;
            case gameScreen::Menu:
                MenuScreen(false);
                break;
            default:
                break;
        }

        Rectangle(0, 0, ScreenWidth, ScreenHeight, 0x000000CC, 1); // Draw a black rectangle over it

        Rectangle(0, 78, ScreenWidth, 2, 0x848284FF, 1);

        Rectangle(0, 383, ScreenWidth, 2, 0x848284FF, 1);
        Rectangle(0, 385, ScreenWidth, 95, 0x000000FF, 1);

        CopiedImg->CopyScreen();
        Copied = true;
    }
    else
    {
        CopiedImg->Draw(0, 0);
    }

    if(GRRLIB_PtInRect(0, 0, ScreenWidth, 78, Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        Rectangle(0, 0, ScreenWidth, 78, 0x30B6EBFF, 1);
    }
    else
    {
        Rectangle(0, 0, ScreenWidth, 78, 0x000000FF, 1);
    }

    GRRLIB_PrintfTTF(30, 20, DefaultFont, Lang->String("HOME Menu").c_str(), 30, 0xFFFFFFFF);

    ExitButton[0]->SetFocused(false);
    ExitButton[1]->SetFocused(false);
    ExitButton[2]->SetFocused(false);
    if(GRRLIB_PtInRect(0, 0, ScreenWidth, 78, Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        ExitButton[0]->SetFocused(true);
        ButtonOn(0);
        FocusedButton = 0;
    }
    else if(ExitButton[1]->IsInside(Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        ExitButton[1]->SetFocused(true);
        ButtonOn(1);
        FocusedButton = 1;
    }
    else if(ExitButton[2]->IsInside(Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        ExitButton[2]->SetFocused(true);
        ButtonOn(2);
        FocusedButton = 2;
    }
    else
    {
        FocusedButton = -1;
    }
    ExitButton[0]->Paint();
    ExitButton[1]->Paint();
    ExitButton[2]->Paint();
}

/**
 * Draw the menu screen.
 * @param[in] CopyScreen If true the screen is copied to a buffer.
 */
void Game::MenuScreen(bool CopyScreen)
{
    if(Copied == false)
    {   // Copy static element
        FillScreen(0x000000FF); // Clear screen
        for(int y = 0; y <= ScreenHeight; y+=8)
        {
            Rectangle(0, y, ScreenWidth, 2, 0xB0B0B030, 1);
        }

        Rectangle(0, 0, ScreenWidth, 63, 0x000000FF, 1);
        Rectangle(0, 63, ScreenWidth, 2, 0xFFFFFFFF, 1);

        Rectangle(0, 383, ScreenWidth, 2, 0xFFFFFFFF, 1);
        Rectangle(0, 385, ScreenWidth, 95, 0x000000FF, 1);

        GRRLIB_PrintfTTF(500, 40, DefaultFont,
            std::format(std::runtime_format(Lang->String("Ver. {}")), "1.1.0").c_str(),
            12, 0xFFFFFFFF);

        if(CopyScreen == true)
        {
            CopiedImg->CopyScreen();
            Copied = true;
        }
    }
    else
    {
        CopiedImg->Draw(0, 0);
    }

    MenuButton[0]->SetFocused(false);
    MenuButton[1]->SetFocused(false);
    MenuButton[2]->SetFocused(false);
    if(MenuButton[0]->IsInside(Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        MenuButton[0]->SetFocused(true);
        ButtonOn(0);
        FocusedButton = 0;
    }
    else if(MenuButton[1]->IsInside(Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        MenuButton[1]->SetFocused(true);
        ButtonOn(1);
        FocusedButton = 1;
    }
    else if(MenuButton[2]->IsInside(Hand[0].GetLeft(), Hand[0].GetTop()) == true)
    {
        MenuButton[2]->SetFocused(true);
        ButtonOn(2);
        FocusedButton = 2;
    }
    else
    {
        FocusedButton = -1;
    }
    MenuButton[0]->Paint();
    MenuButton[1]->Paint();
    MenuButton[2]->Paint();
}

/**
 * Controls all inputs.
 * @return True to exit to loader, false otherwise.
 */
bool Game::ControllerManager()
{
    RUMBLE_Verify();
    WPADData *WPadData0 = WPAD_Data(WPAD_CHAN_0);
    WPADData *WPadData1 = WPAD_Data(WPAD_CHAN_1);
    WPADData *WPadData2 = WPAD_Data(WPAD_CHAN_2);
    WPADData *WPadData3 = WPAD_Data(WPAD_CHAN_3);
    const u32 Buttons0 = WPadData0->btns_d;
    const u32 Buttons1 = WPadData1->btns_d;
    const u32 Buttons2 = WPadData2->btns_d;
    const u32 Buttons3 = WPadData3->btns_d;

    if(WPadData0->ir.valid)
    {
        // I don't understand this calculation but it works
        Hand[0].SetLeft((WPadData0->ir.x / ScreenWidth * (ScreenWidth + Hand[0].GetWidth() * 2)) - Hand[0].GetWidth());
        Hand[0].SetTop((WPadData0->ir.y / ScreenHeight * (ScreenHeight + Hand[0].GetHeight() * 2)) - Hand[0].GetHeight());
        Hand[0].SetAngle(WPadData0->orient.roll);
        Hand[0].SetVisible(true);
    }
    else
    {   // Hide cursor
        Hand[0].SetVisible(false);
    }
    if(WPadData1->ir.valid && (CurrentScreen == gameScreen::Home ||
        CurrentScreen == gameScreen::Start || CurrentScreen == gameScreen::Menu ||
        (CurrentScreen == gameScreen::Game && GameMode == gameMode::VsHuman2) ))
    {
        // I don't understand this calculation but it works
        Hand[1].SetLeft((WPadData1->ir.x / ScreenWidth * (ScreenWidth + Hand[1].GetWidth() * 2)) - Hand[1].GetWidth());
        Hand[1].SetTop((WPadData1->ir.y / ScreenHeight * (ScreenHeight + Hand[1].GetHeight() * 2)) - Hand[1].GetHeight());
        Hand[1].SetAngle(WPadData1->orient.roll);
        Hand[1].SetVisible(true);
    }
    else
    {   // Hide cursor
        Hand[1].SetVisible(false);
    }
    if(WPadData2->ir.valid && (CurrentScreen == gameScreen::Home))
    {
        // I don't understand this calculation but it works
        Hand[2].SetLeft((WPadData2->ir.x / ScreenWidth * (ScreenWidth + Hand[2].GetWidth() * 2)) - Hand[2].GetWidth());
        Hand[2].SetTop((WPadData2->ir.y / ScreenHeight * (ScreenHeight + Hand[2].GetHeight() * 2)) - Hand[2].GetHeight());
        Hand[2].SetAngle(WPadData2->orient.roll);
        Hand[2].SetVisible(true);
    }
    else
    {   // Hide cursor
        Hand[2].SetVisible(false);
    }
    if(WPadData3->ir.valid && (CurrentScreen == gameScreen::Home))
    {
        // I don't understand this calculation but it works
        Hand[3].SetLeft((WPadData3->ir.x / ScreenWidth * (ScreenWidth + Hand[3].GetWidth() * 2)) - Hand[3].GetWidth());
        Hand[3].SetTop((WPadData3->ir.y / ScreenHeight * (ScreenHeight + Hand[3].GetHeight() * 2)) - Hand[3].GetHeight());
        Hand[3].SetAngle(WPadData3->orient.roll);
        Hand[3].SetVisible(true);
    }
    else
    {   // Hide cursor
        Hand[3].SetVisible(false);
    }

    if(Buttons0 || Buttons1)
    {
        switch(CurrentScreen)
        {
            case gameScreen::Start:
                if((Buttons0 & WPAD_BUTTON_A))
                {
                    ChangeScreen(gameScreen::Menu);
                }
                break;
            case gameScreen::Menu:
                if((Buttons0 & WPAD_BUTTON_A))
                {
                    switch(FocusedButton)
                    {
                        case 0:
                            WTTPlayer[1].SetType(playerType::Human);
                            GameMode = gameMode::VsHuman1;
                            ChangeScreen(gameScreen::Game);
                            break;
                        case 1:
                            WTTPlayer[1].SetType(playerType::CPU);
                            GameMode = gameMode::VsAI;
                            ChangeScreen(gameScreen::Game);
                            break;
                        case 2:
                            WTTPlayer[1].SetType(playerType::Human);
                            GameMode = gameMode::VsHuman2;
                            ChangeScreen(gameScreen::Game);
                            break;
                        default:
                            break;
                    }
                }
                else if((Buttons0 & WPAD_BUTTON_B))
                {
                    ChangeScreen(gameScreen::Start);
                }
                else if((Buttons0 & WPAD_BUTTON_HOME) || (Buttons1 & WPAD_BUTTON_HOME))
                {
                    ChangeScreen(gameScreen::Home);
                }
                break;
            case gameScreen::Home:
                if((Buttons0 & WPAD_BUTTON_HOME) || (Buttons1 & WPAD_BUTTON_HOME))
                {
                    GameAudio->PauseMusic(false);
                    ChangeScreen(LastScreen);
                }
                else if((Buttons0 & WPAD_BUTTON_A))
                {
                    switch(FocusedButton)
                    {
                        case 0:
                            GameAudio->PauseMusic(false);
                            ChangeScreen(LastScreen);
                            break;
                        case 1:
                            NewGame();
                            break;
                        case 2:
                            ExitScreen();
                            Hand[3].Paint();
                            Hand[2].Paint();
                            Hand[1].Paint();
                            Hand[0].Paint();
                            CopiedImg->CopyScreen();
                            WPAD_Rumble(WPAD_CHAN_ALL, 0); // Rumble off, just in case
                            Draw_FadeOut(CopiedImg.get(), 1, 1, 3);
                            return true; // Exit to loader
                        default:
                            break;
                    }
                }
                break;
            default:
                if((Buttons0 & WPAD_BUTTON_HOME) || (Buttons1 & WPAD_BUTTON_HOME))
                {
                    ChangeScreen(gameScreen::Home);
                }

                if((Buttons0 & WPAD_BUTTON_A))
                {
                    if(FocusedButton > -1)
                    {
                        ChangeScreen(gameScreen::Home);
                    }
                    else if(RoundFinished == true)
                    {
                        Clear();
                    }
                    else if(GameMode == gameMode::VsHuman2)
                    {
                        if(CurrentPlayer == 0)
                        {
                            if(GameGrid->SetPlayer(WTTPlayer[0].GetSign(), HandX, HandY) == true)
                            {
                                TurnIsOver();
                            }
                            else
                            {   // Position is invalid
                                RUMBLE_Wiimote(WPAD_CHAN_0, 200); // 200 ms
                            }
                        }
                    }
                    else
                    {
                        if(GameGrid->SetPlayer(WTTPlayer[CurrentPlayer].GetSign(), HandX, HandY) == true)
                        {
                            TurnIsOver();
                        }
                        else
                        {   // Position is invalid
                            RUMBLE_Wiimote(WPAD_CHAN_0, 200); // 200 ms
                        }
                    }
                }

                if((Buttons1 & WPAD_BUTTON_A) && GameMode == gameMode::VsHuman2)
                {
                    if(RoundFinished == true)
                    {
                        Clear();
                    }
                    else if(CurrentPlayer == 1)
                    {
                        if(GameGrid->SetPlayer(WTTPlayer[1].GetSign(), HandX, HandY) == true)
                        {
                            TurnIsOver();
                        }
                        else
                        {   // Position is invalid
                            RUMBLE_Wiimote(WPAD_CHAN_1, 200); // 200 ms
                        }
                    }
                }
                break;
        }
    }
    if(((WPadData0->btns_h & WPAD_BUTTON_1) && (WPadData0->btns_h & WPAD_BUTTON_2)) ||
        ((WPadData1->btns_h & WPAD_BUTTON_1) && (WPadData1->btns_h & WPAD_BUTTON_2)) ||
        ((WPadData2->btns_h & WPAD_BUTTON_1) && (WPadData2->btns_h & WPAD_BUTTON_2)) ||
        ((WPadData3->btns_h & WPAD_BUTTON_1) && (WPadData3->btns_h & WPAD_BUTTON_2)))
    {
        WPAD_Rumble(WPAD_CHAN_ALL, 1); // Rumble on
        WIILIGHT_TurnOn();

        const auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
        const auto path = std::format("sd:/Screenshot {:%F %H%M%S}.png", now);

        if(ScreenShot(path) == true)
        {
            text = "A screenshot was taken!!!";
        }
        else
        {
            text = "Screenshot did not work!!!";
        }

        WIILIGHT_TurnOff();
        WPAD_Rumble(WPAD_CHAN_ALL, 0); // Rumble off
        Copied = false;
    }
    if((Buttons0 & WPAD_BUTTON_PLUS) || (Buttons1 & WPAD_BUTTON_PLUS) ||
        (Buttons2 & WPAD_BUTTON_PLUS) || (Buttons3 & WPAD_BUTTON_PLUS))
    {
        ShowFPS = !ShowFPS;
    }
    return false;
}

/**
 * Start a new round.
 */
void Game::Clear()
{
    GameGrid->Clear();
    CurrentPlayer = PlayerToStart;
    PlayerToStart = !PlayerToStart; // Next other player will start
    text = std::format(std::runtime_format(Lang->GetTurnOverMessage()), WTTPlayer[CurrentPlayer].GetName());
    RoundFinished = false;
    Copied = false;
    ChangeCursor();
}

/**
 * Things to do when a turn is over.
 */
void Game::TurnIsOver()
{
    u8 GameWinner = GameGrid->GetWinner();
    if(GameWinner != ' ')
    {   // A winner is declare
        GameWinner = (GameWinner == WTTPlayer[0].GetSign()) ? 0 : 1;
        WTTPlayer[GameWinner].IncScore();
        text = std::format(std::runtime_format(Lang->GetWinningMessage()),
            WTTPlayer[GameWinner].GetName(), WTTPlayer[!GameWinner].GetName());
        RoundFinished = true;
        SymbolAlpha = 5;
        AlphaDirection = false;
    }
    else if(GameGrid->IsFilled() == true)
    {   // Tie game
        ++TieGame;
        text = Lang->GetTieMessage();
        RoundFinished = true;
    }
    else
    {
        CurrentPlayer = !CurrentPlayer; // Change player's turn
        text = std::format(std::runtime_format(Lang->GetTurnOverMessage()), WTTPlayer[CurrentPlayer].GetName());
    }

    Copied = false;
    ChangeCursor();
}

/**
 * Start a new game, initialize variables.
 */
void Game::NewGame()
{
    GameAudio->LoadMusic();

    PlayerToStart = rand() & 1; // 0 or 1

    ChangeScreen(gameScreen::Start, false);

    WTTPlayer[0].ResetScore();
    WTTPlayer[1].ResetScore();
    TieGame = 0;

    AIThinkLoop = 0;

    Clear();
}

/**
 * Print the text with multiple lines if needed.
 * @param[in] x Specifies the x-coordinate of the upper-left corner of the text.
 * @param[in] y Specifies the y-coordinate of the upper-left corner of the text.
 * @param[in] maxLineWidth Maximum width of the string.
 * @param[in] input Text to draw.
 * @param[in] fontSize Size of the text.
 * @param[in] TextColor Text color in RGBA format. The alpha channel is used to change the opacity of the text.
 * @param[in] ShadowColor Shadow color in RGBA format. The alpha channel is used to change the opacity of the text.
 * @param[in] OffsetX Shadow offset for the x-coordinate.
 * @param[in] OffsetY Shadow offset for the y-coordinate.
 */
void Game::PrintWrapText(u16 x, u16 y, u16 maxLineWidth,
    const std::string &input, u32 fontSize, u32 TextColor,
    u32 ShadowColor, s8 OffsetX, s8 OffsetY)
{
    std::string tmp = input + " "; // Make local copy
    auto startIndex = tmp.begin();
    auto lastSpace = tmp.begin();
    int ypos = y;
    int z = 0;
    const int stepSize = (fontSize * 1.2);

    for(auto i = tmp.begin(); i != tmp.end(); ++i)
    {
        if(*i == ' ')
        {
            const std::string tmp2(startIndex, i);
            z = GRRLIB_WidthTTF(DefaultFont, tmp2.c_str(), fontSize);

            if(z >= maxLineWidth)
            {
                *lastSpace = 0;
                const int textLeft = x + (maxLineWidth / 2.0) -
                    (GRRLIB_WidthTTF(DefaultFont, &(*startIndex), fontSize) / 2.0);
                GRRLIB_PrintfTTF(textLeft + OffsetX, ypos + OffsetY, DefaultFont, &(*startIndex),
                    fontSize, ShadowColor);
                GRRLIB_PrintfTTF(textLeft, ypos, DefaultFont, &(*startIndex),
                    fontSize, TextColor);
                startIndex = lastSpace + 1;
                ypos += stepSize;
                z = 0;
            }
            lastSpace = i;
        }
    }
    if(z <= maxLineWidth)
    {
        const int textLeft = x + (maxLineWidth / 2.0) -
            (GRRLIB_WidthTTF(DefaultFont, &(*startIndex), fontSize) / 2.0);
        GRRLIB_PrintfTTF(textLeft + OffsetX, ypos + OffsetY, DefaultFont, &(*startIndex),
            fontSize, ShadowColor);
        GRRLIB_PrintfTTF(textLeft, ypos, DefaultFont, &(*startIndex),
            fontSize, TextColor);
    }
}

/**
 * Change the screen.
 * @param[in] NewScreen New screen to show.
 * @param[in] PlaySound Set to true to play a sound. Default value is true.
 */
void Game::ChangeScreen(gameScreen NewScreen, bool PlaySound)
{
    if(PlaySound == true)
    {
        GameAudio->PlaySoundScreenChange(100);
    }

    FocusedButton = -1;
    LastScreen = CurrentScreen;
    CurrentScreen = NewScreen;

    Copied = false;
    ChangeCursor();
}

/**
 * Rumble if on button.
 * @param[in] NewFocusedButton New button to select.
 */
void Game::ButtonOn(s8 NewFocusedButton)
{
    if(FocusedButton != NewFocusedButton)
    {
        GameAudio->PlaySoundButton(80);
        RUMBLE_Wiimote(WPAD_CHAN_0, 50); // 50 ms
    }
}

/**
 * Check if a zone is selected.
 * @return true if in a zone, false otherwise.
 */
bool Game::SelectZone()
{
    u8 HandID = 0;
    if(GameMode == gameMode::VsHuman2 && CurrentPlayer == 1)
    {
        HandID = 1;
    }
    if(RoundFinished == false && AIThinkLoop == 0)
    {
        for(s8 x = 0; x < 3; ++x)
        {
            for(s8 y = 0; y < 3; ++y)
            {
                if (Hand[HandID].GetLeft() > Table[x][y].GetX() &&
                    Hand[HandID].GetLeft() < (Table[x][y].GetX() + 136) &&
                    Hand[HandID].GetTop() > Table[x][y].GetY() &&
                    Hand[HandID].GetTop() < (Table[x][y].GetY() + 100))
                {
                    if(HandX != x || HandY != y)
                    {
                        HandX = x;
                        HandY = y;
                        if(GameGrid->GetPlayerAtPos(HandX, HandY) == ' ')
                        {   // Zone is empty
                            GameAudio->PlaySoundButton(90);
                            RUMBLE_Wiimote(HandID, 30); // 30 ms
                        }
                    }
                    return true;
                }
            }
        }
    }
    HandX = -1;
    HandY = -1;
    return false;
}

/**
 * Change the cursor.
 */
void Game::ChangeCursor()
{
    if(CurrentScreen == gameScreen::Home)
    {
        Hand[0].SetPlayer(cursorType::P1);
        Hand[0].SetAlpha(0xFF);
        Hand[1].SetPlayer(cursorType::P2);
        Hand[1].SetAlpha(0x55);
        Hand[2].SetPlayer(cursorType::P3);
        Hand[2].SetAlpha(0x55);
        Hand[3].SetPlayer(cursorType::P4);
        Hand[3].SetAlpha(0x55);
    }
    else if(CurrentScreen == gameScreen::Game)
    {
        if(GameMode == gameMode::VsHuman1)
        {
            if(WTTPlayer[CurrentPlayer].GetSign() == 'O')
            {
                Hand[0].SetPlayer(cursorType::O);
            }
            else if(WTTPlayer[CurrentPlayer].GetSign() == 'X')
            {
                Hand[0].SetPlayer(cursorType::X);
            }
            Hand[0].SetAlpha(0xFF);
        }
        else if(GameMode == gameMode::VsHuman2)
        {
            Hand[0].SetPlayer(cursorType::X);
            Hand[1].SetPlayer(cursorType::O);
            if(RoundFinished == true)
            {
                Hand[0].SetAlpha(0xFF);
                Hand[1].SetAlpha(0xFF);
            }
            else
            {
                Hand[CurrentPlayer].SetAlpha(0xFF);
                Hand[!CurrentPlayer].SetAlpha(0x55);
            }
        }
        else
        {   // gameMode::VsAI
            Hand[0].SetPlayer(cursorType::X);
            if(CurrentPlayer == 0 || RoundFinished == true)
            {
                Hand[0].SetAlpha(0xFF);
                Hand[1].SetAlpha(0x55);
            }
            else
            {
                Hand[0].SetAlpha(0x55);
                Hand[1].SetAlpha(0xFF);
            }
        }
    }
    else
    {   // START_SCREEN or MENU_SCREEN
        Hand[0].SetPlayer(cursorType::X);
        Hand[0].SetAlpha(0xFF);
        Hand[1].SetPlayer(cursorType::O);
        Hand[1].SetAlpha(0x55);
    }
}

/**
 * This function calculates the number of frames we render each second.
 */
void Game::CalculateFrameRate()
{
    static u8 frameCount = 0;
    static u32 lastTime;
    const u32 currentTime = ticks_to_millisecs(gettime());

    ++frameCount;
    if(currentTime - lastTime > 1000)
    {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
}
