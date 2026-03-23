#include "HomeScene.h"
#include "Global.h"
#include "DirectX.h"
#include"GameMain.h"


using namespace Global;
using namespace Global::Window;

bool HomeScene::Create_Background()
{
    background = LoadSurface(Resource::Home::Backgroud);
    return background != NULL;
}

void HomeScene::Draw_Background()
{
    RECT source_rect = {
        0,
        0,
        ScreenWidth,
        ScreenHeight
    };
    d3dDev->StretchRect(background, NULL, backBuffer, &source_rect, D3DTEXF_NONE);
}

bool HomeScene::Init()
{
    OutputDebugString("欢迎场景开始初始化\n");
    if (!HomeScene::Create_Background())
    {
        ShowMessage("背景图载入失败");
        return false;
    }

    HomeScene::choose = 0;
    HomeScene::font = MakeFont("微软雅黑", 32);
    //DXSound组件是软解码，库里暂时只支持wav
    //if (bgm = LoadSound(Resource::Home::BGM), bgm == NULL) {
        //ShowMessage("BGM载入失败");
        //return false;
    //}
    //LoopSound(bgm);


    return true;
}
void HomeScene::End()
{
    //释放背景图
    background->Release();
    //delete bgm;
}
void HomeScene::Update()
{
    //if (Mouse_Button(MLButton))
    //{
    //    OutputDebugString("左键单击");
    //}

    if (Key_Up(DIK_DOWN))
    {
        HomeScene::choose++;
        HomeScene::choose %= 5;
    }
    if (Key_Up(DIK_UP))
    {
        HomeScene::choose = HomeScene::choose > 0 ? HomeScene::choose - 1 : 4;
        HomeScene::choose %= 5;
    }
    if (Key_Up(DIK_SPACE))
    {
        Global::Home::selectedType = choose;
		Global::Window::Now_GAME_STATE = 1;
        switch (choose)
        {
        case 0:
        case 1://双人游戏和单人游戏的接口一样
			Game_ChangeScene(GAME_STATE::SinglePlayer);
            break;
		case 2:
			Game_ChangeScene(GAME_STATE::DesignMap);
			break;
		case 3:
			Game_ChangeScene(GAME_STATE::GameSatting);
			break;
        case 4:
            Game_ChangeScene(GAME_STATE::About);
            break;

        default:
            break;
        }
    }
}
void HomeScene::Render()
{
    HomeScene::Draw_Background();
    for (int i = 0; i < 5; i++) {
        FontPrint(font, 450, 700 + i * 40, Resource::Home::OptionsStr[i], i == HomeScene::choose ? Resource::Home::SelectedColor : Resource::Home::UnselectedColor);
    }
}