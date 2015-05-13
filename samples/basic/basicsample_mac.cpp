
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_MAC)
#include <iostream>


// Define your user buttons
enum Button
{
	ButtonMenu,
	ButtonConfirm,
	MouseX,
	MouseY
};


const char* windowName = "Gainput basic sample";
const int width = 800;
const int height = 600;


int main(int argc, char** argv)
{
	std::cout << "Not implemented." << std::endl;
	return 0;
}
#endif

