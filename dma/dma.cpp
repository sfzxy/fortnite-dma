// idk
bool started = false;
bool die = false;

// KMBOX NET
#include "NetConfig/kmboxNet.h"
#include "NetConfig/HidTable.h"

// KMBOX SERIAL
#include "kmboxcom.h"

// IO
#include <iostream>
#include <fstream>
#include "console.h"

// functions
#include <cmath>
#include <numeric>

// KMBOX SERIAL
#include "kmbox_interface.h"

inline void km_move(int X, int Y) {
	std::string command = "km.move(" + std::to_string(X) + "," + std::to_string(Y) + ")\r\n";
	send_command(hSerial, command.c_str());
}

inline void km_click(int clickDelay) {
	std::string command = "km.left(" + std::to_string(1) + ")\r\n";
	send_command(hSerial, command.c_str());
	Sleep(clickDelay);
	std::string command1 = "km.left(" + std::to_string(0) + ")\r\n";
	send_command(hSerial, command1.c_str());
}

// DMA
#include "DMAHandler.h"

// window
#include "render.h"
#include "render/ImGui/imgui_internal.h"
#include "render/cutome/etc_element.h"

#include "radar.h"

DMAHandler* TargetProcess; // FortniteClient-Win64-Shipping.exe

// fortnite
#include "offsets.h"
#include "pointers.h"
#include "colors.h"
#include "settings.h"
#include "gameFunctions.h"

#include "loop.h"

int main()
{
	//SetThreadPriority(GetCurrentProcess(), THREAD_PRIORITY_TIME_CRITICAL);

	SetConsoleTitleA("Asgard");

	std::cout << " - welcome \n";

	std::cout << " - checking presence of serial kmbox\n";

	string port = find_port(("USB-SERIAL CH340"));

	if (!port.empty()) {

		if (open_port(hSerial, port.c_str(), CBR_115200)) {
			std::cout << " - serial kmbox found \n";
			SETTINGS::kmboxserial = true;
		}
		
	}
	else {
		std::cout << " - serial kmbox not found \n";
	}

	if (!SETTINGS::kmboxserial)
	{
		char choice;

		std::cout << " - use kmbox net?(y/n) : ";

		std::cin >> choice;


		if (choice == 'y' || choice == 'Y') {

			std::ifstream ipFile(std::string("kmboxnet/ip.txt"));

			if (!ipFile.is_open()) {
				std::cout << " - ip file not found \n";
				Sleep(5000);
				(exit)(0);
			}

			std::ifstream portFile(std::string("kmboxnet/port.txt"));

			if (!portFile.is_open()) {
				std::cout << " - port file not found \n";
				Sleep(5000);
				(exit)(0);
			}

			std::ifstream uuidFile(std::string("kmboxnet/uuid.txt"));

			if (!uuidFile.is_open()) {
				std::cout << " - uuid file not found \n";
				Sleep(5000);
				(exit)(0);
			}

			int resultNet = kmNet_init((char*)(std::string((std::istreambuf_iterator<char>(ipFile)), std::istreambuf_iterator<char>()).c_str()), (char*)(std::string((std::istreambuf_iterator<char>(portFile)), std::istreambuf_iterator<char>()).c_str()), (char*)(std::string((std::istreambuf_iterator<char>(uuidFile)), std::istreambuf_iterator<char>()).c_str()));

			if (resultNet == err_net_rx_timeout || resultNet == err_net_version) {
			}
			else
			{
				std::cout << " - kmbox net found \n";
				SETTINGS::kmboxnet = true;
				kmNet_monitor(1);
			}
		}
	}

	std::cout << " - looking for the game \n";

	TargetProcess = new DMAHandler(L"FortniteClient-Win64-Shipping.exe", false);

	if (!TargetProcess->IsInitialized())
	{ 
		std::cout << " - failed to initialize \n";
		(exit)(0);
	}

	if (!TargetProcess->GetPID())
	{
		std::cout << " - game not found \n";
		std::cin.get();
		(exit)(0);
	}

	std::cout << " - game found \n";

	if (!TargetProcess->FixDTB())
	{
		std::cout << " - failed to fix dtb \n";
		Sleep(5000);
		(exit)(0);
	}
	else {
		SetConsoleTitleA("Asgard");
	}

	if (SETTINGS::kmboxnet || SETTINGS::kmboxserial)
	{
		if (!TargetProcess->InitKeyboard())
		{
			std::cout << " - failed to initialize keyboard \n";
		}
		else {
			std::cout << " - keyboard initialized \n";
			SETTINGS::getKeyReady = true;
		}

		std::cout << " - system 1 online \n";
		HANDLE aimThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)aim, nullptr, NULL, nullptr);

		if (aimThread != NULL)
		{
			CloseHandle(aimThread);
		}
	}

	std::cout << " - system 2 online \n";
	HANDLE newInfoThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)newInfo, nullptr, NULL, nullptr);

	if (newInfoThread != NULL)
	{
		CloseHandle(newInfoThread);
	}

	std::cout << " - system 3 online \n";
	HANDLE cameraThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)cameraUpdates, nullptr, NULL, nullptr);

	if (cameraThread != NULL)
	{
		CloseHandle(cameraThread);
	}

	std::cout << " - system 4 online \n";
	HANDLE LocalPlayerThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)LocalPlayerUpdates, nullptr, NULL, nullptr);

	if (LocalPlayerThread != NULL)
	{
		CloseHandle(LocalPlayerThread);
	}

	std::cout << " - system 5 online \n";
	HANDLE pLoopThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)pLoop, nullptr, NULL, nullptr);

	if (pLoopThread != NULL)
	{
		CloseHandle(pLoopThread);
	}

	std::cout << " - system 6 online \n";
	HANDLE iLoopThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)iLoop, nullptr, NULL, nullptr);

	if (iLoopThread != NULL)
	{
		CloseHandle(iLoopThread);
	}

	point::Base = TargetProcess->GetBaseAddress();

	getVaText();

	Load_Settings(("C:\\FNCFG"));

	started = true;

	std::cout << " - creating window \n";

	Render::CreateOverlay();
	Render::DirectXInit();

	std::cout << " - booting \n";

	while (true)
	{
		MainLoop();
	}

	std::cin.get();
	exit(0);
}