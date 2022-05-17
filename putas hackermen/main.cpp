#include <Windows.h>

#include "gui/base.hpp"

int WINAPI WinMain(
	HINSTANCE hInst,
	HINSTANCE, /* unused */
	PSTR, /* unused */
	int) /* unused */
{
	gui::CreateGuiApp("Putas Hackermen", "Piroca Negra");
	gui::Main();

	return TRUE;
}