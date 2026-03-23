#include "CursorGUI.h"
#include"DirectX.h"

namespace GUI {
    namespace Cursor {
        LPDIRECT3DTEXTURE9 Cursor_Texture = NULL;
        SPRITE Cursor;

        //将当前鼠标位置更新到鼠标精灵位置
        void Update() {
            Cursor.x = mousePoint.x;
            Cursor.y = mousePoint.y;
        }
        void Render() {
            Sprite_Transform_Draw(
                Cursor_Texture,
                Cursor.x, Cursor.y,
                Cursor.width,
                Cursor.height,
                0, 1, 0.0f, 1.0f,
                D3DCOLOR_XRGB(255, 255, 255));
        }
        bool Init()
        {
            D3DXIMAGE_INFO Info;
            //得到图片文件信息到Info
            if (D3D_OK != (D3DXGetImageInfoFromFile(Resource::Cursor::Normal, &Info)))
            {
                MessageBox(NULL, "得到图象信息错误!", "LOAD PIC ERROR", MB_OK);
            }
            Cursor_Texture = LoadTexture(Resource::Cursor::Normal);
            if (!Cursor_Texture) return false;

            Cursor.x = Cursor.y = 0;
            Cursor.width = Info.Width;
            Cursor.height = Info.Height;
        }
    }
}
                                                                                                                                              