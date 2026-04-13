#include <nanogui/nanogui.h>
#include <iostream>

using namespace nanogui;

int main(int argc, char *argv[])
{
    nanogui::init();

    Screen * screen = new Screen(Vector2i(500, 700), "NanoGUI test");
}