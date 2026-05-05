#include"DebugTools.h"
#include"DirectX.h"
namespace DebugTools {
    string int2str(int int_temp) {
        stringstream stream;
        stream << int_temp;
        return stream.str();
    }
    void PrintMouseInfo() {
        static LPD3DXFONT font = MakeFont("宋体", 18);
        string text;
        text += "Total FPS：";
        text += int2str(Global::Debug::currentFPS);
        text += " Logical FPS：";
        text += int2str(Global::Window::targetFps);
        text += "\n";
        text += "鼠标 X：";
        text += int2str(Mouse_X());
        text += " Y: ";
        text += int2str(Mouse_Y());
        text += "\n";
        FontPrint(font, 0, 0, text, D3DCOLOR_XRGB(255, 255, 255));

    }
}