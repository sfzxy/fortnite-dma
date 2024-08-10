#pragma once

//#define DEBUGOFFSETS

POINT p;

int maximum_distance = 20;
bool has_clicked;
std::chrono::steady_clock::time_point tb_begin;
std::chrono::steady_clock::time_point tb_end;
int tb_time_since;

uint32_t g_nPlayers = 0;
Vector3 g_localPos = Vector3();
uint32_t g_localTeam = NULL;

void getVaText() {
	for (int i = 0; i < 25; i++) {
		if (TargetProcess->Read<__int32>(point::Base + (i * 0x1000) + 0x250) == 0x260E020B) {
			point::va_text = point::Base + ((i + 1) * 0x1000);
		}
	}
}

void newInfo()
{
	while (!started)
	{
		Sleep(1000);
	}
	while (!die) {

		point::Uworld = TargetProcess->Read<__int64>(point::va_text + offsets::Uworld);

#ifdef DEBUGOFFSETS
		std::cout << "point::Uworld: 0x" << std::hex << point::Uworld << std::endl;
#endif
		if (!point::Uworld) continue;

		uintptr_t temp1;
		uintptr_t temp2;
		uintptr_t temp3;
		uintptr_t temp4;
		uintptr_t temp5;

		VMMDLL_SCATTER_HANDLE scatterReadHandle1 = TargetProcess->CreateScatterHandle(0);

		
		TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::Uworld + 0x110, &temp1, sizeof(uintptr_t));
		TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::Uworld + 0x120, &temp2, sizeof(uintptr_t));

		TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::Uworld + offsets::GameInstance, &temp3, sizeof(uintptr_t));
		TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::Uworld + offsets::PersistentLevel, &temp4, sizeof(uintptr_t));
		TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::Uworld + offsets::GameState, &temp5, sizeof(uintptr_t));

		TargetProcess->ExecuteScatterRead(scatterReadHandle1);

		point::LocationPointer = temp1;
		point::RotationPointer = temp2;
		point::GameInstance = temp3;
		point::PersistentLevel = temp4;
		point::GameState = temp5;

		uint32_t fnameKey = TargetProcess->Read<uint32_t>(point::GameState + offsets::temp1);

		std::string name = GetNameFromFName(fnameKey);

#ifdef DEBUGOFFSETS
		std::cout << "point::GameInstance: 0x" << std::hex << point::GameInstance << std::endl;
		std::cout << "point::PersistentLevel: 0x" << std::hex << point::PersistentLevel << std::endl;
		std::cout << "point::GameState: 0x" << std::hex << point::GameState << std::endl;
#endif

		if (!point::GameInstance) continue;

		if (!point::PersistentLevel) continue;

		if (!point::GameState) continue;

		point::LocalPlayers = TargetProcess->Read<uintptr_t>(point::GameInstance + offsets::LocalPlayers);
#ifdef DEBUGOFFSETS
		std::cout << "point::LocalPlayers: 0x" << std::hex << point::LocalPlayers << std::endl;
#endif
		if (!point::LocalPlayers) continue;

		point::LocalPlayer = TargetProcess->Read<uintptr_t>(point::LocalPlayers);
#ifdef DEBUGOFFSETS
		std::cout << "point::LocalPlayer: 0x" << std::hex << point::LocalPlayer << std::endl;
#endif
		if (!point::LocalPlayer) continue;

		point::PlayerController = TargetProcess->Read<uintptr_t>(point::LocalPlayer + offsets::PlayerController);
#ifdef DEBUGOFFSETS
		std::cout << "point::PlayerController: 0x" << std::hex << point::PlayerController << std::endl;
#endif

		if (point::PlayerController) point::Player = TargetProcess->Read<uintptr_t>(point::PlayerController + offsets::LocalPawn);

		if (point::PlayerController) point::PlayerCameraManager = TargetProcess->Read<uintptr_t>(point::PlayerController + offsets::Cameramanager);

		if (point::Player) point::PlayerState = TargetProcess->Read<uintptr_t>(point::Player + offsets::PlayerState);

		uintptr_t tempPlayerArray;
		uint32_t tempnPlayers;

		VMMDLL_SCATTER_HANDLE scatterReadHandle2 = TargetProcess->CreateScatterHandle(0);

		TargetProcess->QueueScatterReadEx(scatterReadHandle2, point::GameState + offsets::PlayerArray, &tempPlayerArray, sizeof(uintptr_t));

		TargetProcess->QueueScatterReadEx(scatterReadHandle2, point::GameState + offsets::PlayerArray + 8, &tempnPlayers, sizeof(uint32_t));

		TargetProcess->ExecuteScatterRead(scatterReadHandle2);

		point::PlayerArray = tempPlayerArray;

		g_nPlayers = tempnPlayers;

#ifdef DEBUGOFFSETS
		std::cout << "point::PlayerArray: 0x" << std::hex << point::PlayerArray << std::endl;
		std::cout << "g_nPlayers: " << std::dec << g_nPlayers << std::endl;
#endif

		if (point::LocationPointer) g_localPos = TargetProcess->Read<Vector3>(point::LocationPointer);

		if (point::PlayerState) g_localTeam = TargetProcess->Read<uint32_t>(point::PlayerState + offsets::TeamId);

		point::ULevelArray = TargetProcess->Read<uintptr_t>(point::Uworld + offsets::ULevelArray);
		point::ULevels = TargetProcess->Read<uint32_t>(point::Uworld + offsets::ULevelCount);

	}
}

Vector3 LUR;
std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
void cameraUpdates()
{
	while (!started)
	{
		Sleep(1000);
	}
	while (!die) {
		updateCamera();
		if (SETTINGS::triggerbot && point::PlayerController) LUR = TargetProcess->Read<Vector3>(point::PlayerController + offsets::LocationUnderReticle);
		if (SETTINGS::KmboxAim ||  SETTINGS::triggerbot) {
			if (std::chrono::system_clock::now() - start > std::chrono::milliseconds(1))
			{
				TargetProcess->UpdateKeys();
				start = std::chrono::system_clock::now();
			}
		}
	}
}

void UpdateLocalPlayerWeapon()
{
	if (!point::Uworld || !point::GameInstance || !point::LocalPlayers || !point::LocalPlayer || !point::PersistentLevel || !point::GameState) return;

	uintptr_t playerWeapon = 0;
	if (point::Player && point::PlayerController) playerWeapon = TargetProcess->Read<uintptr_t>(point::Player + offsets::PlayerWeapon);
	if (playerWeapon) {
		uintptr_t weaponData = TargetProcess->Read<uintptr_t>(playerWeapon + offsets::WeaponData);
		if (weaponData) {
			uintptr_t textPtr = TargetProcess->Read<uintptr_t>(weaponData + offsets::TextPtr);
			if (textPtr) {
				uint64_t ftext_data = TargetProcess->Read<uintptr_t>(textPtr + 0x28);
				int ftext_length = TargetProcess->Read<uintptr_t>(textPtr + 0x30);
				if (ftext_length > 0 && ftext_length < 50) {
					wchar_t* ftext_buf = new wchar_t[ftext_length];
					TargetProcess->ReadString(ftext_data, ftext_buf, ftext_length * sizeof(wchar_t));
					wchar_t* WeaponName = ftext_buf;

					std::wstring wstr_buf(ftext_buf);

					g_localPlayer.localWeaponName = std::string(wstr_buf.begin(), wstr_buf.end());

					delete[] ftext_buf;

					//std::cout << "weapon in hand: " << std::string(wstr_buf.begin(), wstr_buf.end()).c_str() << std::endl;
					bulletSpeed = TargetProcess->Read<float>(playerWeapon + 0x1a78);
					gravity = TargetProcess->Read<float>(playerWeapon + 0x1ce4);
					//printf("speed: %f\n", bulletSpeed);
					//printf("gravity: %f\n", gravity);

					//std::cout << "offsets::speed: 0x" << std::hex << offsets::temp1 << std::endl;
					//std::cout << "playerWeapon: 0x" << std::hex << playerWeapon << std::endl;
					//Vector3 lur = TargetProcess->Read<Vector3>(point::PlayerController + offsets::LocationUnderReticle);
					//printf("locationunderreticle -> x:%f y:%f z:%f\n", lur.x, lur.y, lur.z);

					if (wcsstr(WeaponName, (L"Pickaxe")))
						pickaxe = true;
					else
						pickaxe = false;

					if (wcsstr(WeaponName, (L"Assault Rifle")) || wcsstr(WeaponName, (L"Havoc Suppressed Assault Rifle")) || wcsstr(WeaponName, (L"Warforged Assault Rifle")) || wcsstr(WeaponName, (L"Red-Eye Assault Rifle"))
						|| wcsstr(WeaponName, (L"Suppressed Assault Rifle")) || wcsstr(WeaponName, (L"Striker Burst Rifle")) || wcsstr(WeaponName, (L"Burst Assault Rifle"))
						|| wcsstr(WeaponName, (L"Ranger Assault Rifle")) || wcsstr(WeaponName, (L"Flapjack Rifle")) || wcsstr(WeaponName, (L"Heavy Assault Rifle"))
						|| wcsstr(WeaponName, (L"MK-Seven Assault Rifle")) || wcsstr(WeaponName, (L"MK-Alpha Assault Rifle")) || wcsstr(WeaponName, (L"Combat Assault Rifle"))
						|| wcsstr(WeaponName, (L"Nemesis AR")) || wcsstr(WeaponName, (L"Ambush Striker AR")) || wcsstr(WeaponName, (L"Striker AR")) || wcsstr(WeaponName, (L"Enforcer AR"))
						|| wcsstr(WeaponName, (L"Tactical Assault Rifle")) || wcsstr(WeaponName, (L"Hammer Assault Rifle")) || wcsstr(WeaponName, (L"Sideways Rifle")) || wcsstr(WeaponName, (L"Makeshift Rifle")) || wcsstr(WeaponName, (L"Drum Gun")) || wcsstr(WeaponName, (L"Nisha's Striker AR"))) {
						
						//bulletSpeed = 500.f;
						//gravity = 120.f;
						ShouldUsePrediction = true;

						SETTINGS::KmboxAim = SETTINGS::rifle::KmboxAim;
						SETTINGS::prediction = SETTINGS::rifle::prediction;
						SETTINGS::show_fov = SETTINGS::rifle::show_fov;
						SETTINGS::fov = SETTINGS::rifle::fov;
						SETTINGS::dev_int = SETTINGS::rifle::dev_int;
						SETTINGS::aimSteps = SETTINGS::rifle::aimSteps;
						SETTINGS::key = SETTINGS::rifle::key;
						SETTINGS::aimBone = SETTINGS::rifle::aimBone;
						SETTINGS::aimAtClosestBone = SETTINGS::rifle::aimAtClosestBone;

						SETTINGS::triggerbot = SETTINGS::rifle::triggerbot;
						SETTINGS::TriggerBotDistance = SETTINGS::rifle::TriggerBotDistance;
						SETTINGS::custom_delay = SETTINGS::rifle::custom_delay;
						SETTINGS::clickDelay = SETTINGS::rifle::clickDelay;
						SETTINGS::triggerKey = SETTINGS::rifle::triggerKey;

					}
					else
					if (wcsstr(WeaponName, (L"Smg")) || wcsstr(WeaponName, (L"Ambush Hyper SMG")) || wcsstr(WeaponName, (L"Submachine Gun")) || wcsstr(WeaponName, (L"Combat Smg")) || wcsstr(WeaponName, (L"Pistol")) || wcsstr(WeaponName, (L"Machine Smg"))
						|| wcsstr(WeaponName, (L"Scoped Burst SMG")) || wcsstr(WeaponName, (L"Hyper SMG")) || wcsstr(WeaponName, (L"Thunder Burst SMG")) || wcsstr(WeaponName, (L"Ranger Pistol"))) {

						//bulletSpeed = 500.f;
						//gravity = 120.f;
						ShouldUsePrediction = true;

						SETTINGS::KmboxAim = SETTINGS::smg::KmboxAim;
						SETTINGS::prediction = SETTINGS::smg::prediction;
						SETTINGS::show_fov = SETTINGS::smg::show_fov;
						SETTINGS::fov = SETTINGS::smg::fov;
						SETTINGS::dev_int = SETTINGS::smg::dev_int;
						SETTINGS::aimSteps = SETTINGS::smg::aimSteps;
						SETTINGS::key = SETTINGS::smg::key;
						SETTINGS::aimBone = SETTINGS::smg::aimBone;
						SETTINGS::aimAtClosestBone = SETTINGS::smg::aimAtClosestBone;

						SETTINGS::triggerbot = SETTINGS::smg::triggerbot;
						SETTINGS::TriggerBotDistance = SETTINGS::smg::TriggerBotDistance;
						SETTINGS::custom_delay = SETTINGS::smg::custom_delay;
						SETTINGS::clickDelay = SETTINGS::smg::clickDelay;
						SETTINGS::triggerKey = SETTINGS::smg::triggerKey;

					}
					else
					if (wcsstr(WeaponName, (L"Hunting Rifle")) || wcsstr(WeaponName, (L"Heavy Sniper Rifle")) || wcsstr(WeaponName, (L"Suppressed Sniper Rifle"))
						|| wcsstr(WeaponName, (L"Storm Scout")) || wcsstr(WeaponName, (L"Bolt-Action Sniper Rifle")) || wcsstr(WeaponName, (L"Automatic Sniper Rifle"))
						|| wcsstr(WeaponName, (L"DMR")) || wcsstr(WeaponName, (L"Thermal DMR")) || wcsstr(WeaponName, (L"Hunter Bolt-Action Sniper")) || wcsstr(WeaponName, (L"Huntress DMR"))
						|| wcsstr(WeaponName, (L"Reaper Sniper Rifle")) || wcsstr(WeaponName, (L"Semi-Auto Sniper Rifle"))
						|| wcsstr(WeaponName, (L"Crossbow")) || wcsstr(WeaponName, (L"Mechanical Bow")) || wcsstr(WeaponName, (L"Hand Cannon"))) {

						//bulletSpeed = 400.f;
						//gravity = 120.f;
						ShouldUsePrediction = true;

						SETTINGS::KmboxAim = SETTINGS::sniper::KmboxAim;
						SETTINGS::prediction = SETTINGS::sniper::prediction;
						SETTINGS::show_fov = SETTINGS::sniper::show_fov;
						SETTINGS::fov = SETTINGS::sniper::fov;
						SETTINGS::dev_int = SETTINGS::sniper::dev_int;
						SETTINGS::aimSteps = SETTINGS::sniper::aimSteps;
						SETTINGS::key = SETTINGS::sniper::key;
						SETTINGS::aimBone = SETTINGS::sniper::aimBone;
						SETTINGS::aimAtClosestBone = SETTINGS::sniper::aimAtClosestBone;

						SETTINGS::triggerbot = SETTINGS::sniper::triggerbot;
						SETTINGS::TriggerBotDistance = SETTINGS::sniper::TriggerBotDistance;
						SETTINGS::custom_delay = SETTINGS::sniper::custom_delay;
						SETTINGS::clickDelay = SETTINGS::sniper::clickDelay;
						SETTINGS::triggerKey = SETTINGS::sniper::triggerKey;

					}
					else
					if (wcsstr(WeaponName, (L"Shotgun")) || wcsstr(WeaponName, (L"Frenzy Auto Shotgun")) || wcsstr(WeaponName, (L"Gatekeeper Shotgun")) || wcsstr(WeaponName, (L"Iron Warrior Hammer Pump Shogtun"))) {

						ShouldUsePrediction = false;

						SETTINGS::KmboxAim = SETTINGS::shotgun::KmboxAim;
						SETTINGS::prediction = SETTINGS::shotgun::prediction;
						SETTINGS::show_fov = SETTINGS::shotgun::show_fov;
						SETTINGS::fov = SETTINGS::shotgun::fov;
						SETTINGS::dev_int = SETTINGS::shotgun::dev_int;
						SETTINGS::aimSteps = SETTINGS::shotgun::aimSteps;
						SETTINGS::key = SETTINGS::shotgun::key;
						SETTINGS::aimBone = SETTINGS::shotgun::aimBone;
						SETTINGS::aimAtClosestBone = SETTINGS::shotgun::aimAtClosestBone;

						SETTINGS::triggerbot = SETTINGS::shotgun::triggerbot;
						SETTINGS::TriggerBotDistance = SETTINGS::shotgun::TriggerBotDistance;
						SETTINGS::custom_delay = SETTINGS::shotgun::custom_delay;
						SETTINGS::clickDelay = SETTINGS::shotgun::clickDelay;
						SETTINGS::triggerKey = SETTINGS::shotgun::triggerKey;


					}

				}
			}
		}
	}
}

void LocalPlayerUpdates()
{
	while (!started)
	{
		Sleep(1000);
	}
	while (!die) {
		UpdateLocalPlayerWeapon();
		Sleep(100);
	}
}

void DrawCircle(int x, int y, int radius, ImU32 color, int segments, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x, y), radius, color, segments);
}

void OutlinedText(int x, int y, ImColor Color, const char* text)
{
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x - 1, y), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x + 1, y), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y - 1), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y + 1), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), Color, text);
}

void CornerBox(int x, int y, int w, int h, ImColor color, int thickness) {
	float line_w = (w / SETTINGS::corner_box_length);
	float line_h = (h / SETTINGS::corner_box_hight);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + line_h), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + line_w, y), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - line_w, y), ImVec2(x + w, y), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + line_h), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - line_h), ImVec2(x, y + h), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + line_w, y + h), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - line_w, y + h), ImVec2(x + w, y + h), color, SETTINGS::box_thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - line_h), ImVec2(x + w, y + h), color, SETTINGS::box_thickness);
}

void NormalBox(int x, int y, int w, int h, ImColor color, int thickness) {
	float rounding = SETTINGS::rounding;
	float line_w = (w / SETTINGS::box_length);
	float line_h = (h / SETTINGS::box_hight);

	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2(x, y),
		ImVec2(x + w, y + h),
		color,
		rounding,
		ImDrawCornerFlags_All,
		SETTINGS::box_thickness
	);
}

void NormalboxPreview(float X, float Y, float W, float H, const ImU32& color, float rounding, int thickness) {

	ImGui::GetForegroundDrawList()->AddRect(
		ImVec2(X, Y),
		ImVec2(X + W, Y + H),
		ImGui::GetColorU32(color),
		rounding,
		ImDrawCornerFlags_All,
		thickness
	);
}

bool IsInScreen(Vector3 pos, int over = 30) {
	if (pos.x > -over && pos.x < Width + over && pos.y > -over && pos.y < Height + over) {
		return true;
	}
	else {
		return false;
	}
}

auto IsShootable(Vector3 lur, Vector3 wl) -> bool {
	if (lur.x >= wl.x - 20 && lur.x <= wl.x + 20 && lur.y >= wl.y - 20 && lur.y <= wl.y + 20)
		return true;
	else
		return false;
}

__forceinline auto is_visible(uintptr_t mesh) -> bool
{
	float last_submit;
	float last_render;

	VMMDLL_SCATTER_HANDLE scatterReadHandle2 = TargetProcess->CreateScatterHandle(0);

	TargetProcess->QueueScatterReadEx(scatterReadHandle2, mesh + offsets::LastSubmitTime, &last_submit, sizeof(float));
	TargetProcess->QueueScatterReadEx(scatterReadHandle2, mesh + offsets::LastRenderTimeOnScreen, &last_render, sizeof(float));

	TargetProcess->ExecuteScatterRead(scatterReadHandle2);

	//printf("last_submit: %f\n", last_submit);
	//printf("last_render: %f\n", last_render);

	const float fVisionTick = 0.06f;
	bool visible = last_render + fVisionTick >= last_submit;
	return visible;
}

#include <codecvt>
#include <functional>

std::string GetPlayerName(__int64 PlayerState)
{
	WORD* pNameBufferPointer;
	int i; // ecx
	char v25; // al
	int v26; // er8
	int v29; // eax

	uint64_t FString = TargetProcess->Read<uint64_t>(PlayerState + offsets::NameOffset);
	if (!FString)
		return std::string(("Bot"));

	int Length = TargetProcess->Read<int>(FString + 16);
	if (Length <= 0 || Length > 30)
		return std::string(("Bot"));

	uint64_t FText = TargetProcess->Read<uint64_t>(FString + 8);
	if (!FText)
		return std::string(("Bot"));

	char16_t* NameBuffer = new char16_t[Length];
	TargetProcess->ReadString(FText, NameBuffer, 2 * Length);
	
	v25 = Length - 1;
	v26 = 0;
	pNameBufferPointer = (WORD*)NameBuffer;

	for (i = (v25) & 3; ; *pNameBufferPointer++ += i & 7)
	{
		v29 = Length - 1;
		if (!(DWORD)Length)
			v29 = 0;

		if (v26 >= v29)
			break;

		i += 3;
		++v26;
	}

	std::u16string Temp{ NameBuffer };

	delete[] NameBuffer;

	return std::string(Temp.begin(), Temp.end());
}


float col[3] = { 1.0f, 0.0f, 1.0f };
ImColor color = ImColor(col[0], col[1], col[2]);
float col3df[3] = { 1.0f, 0.0f, 1.0f };
ImColor col3d = ImColor(col3df[0], col3df[1], col3df[2]);
float col2df[3] = { 1.0f, 0.0f, 1.0f };
ImColor col2d = ImColor(col2df[0], col2df[1], col2df[2]);
float colSKf[3] = { 1.0f, 0.0f, 1.0f };
ImColor colSK = ImColor(colSKf[0], colSKf[1], colSKf[2]);
float collinesf[3] = { 1.0f, 0.0f, 1.0f };
ImColor collines = ImColor(collinesf[0], collinesf[1], collinesf[2]);
float colaimlinef[3] = { 1.0f, 1.0f, 1.0f };
ImColor colaimline = ImColor(colaimlinef[0], colaimlinef[1], colaimlinef[2]);
float colfovf[3] = { 1.0f, 0.0f, 1.0f };
ImColor colfov = ImColor(colfovf[0], colfovf[1], colfovf[2]);

float coldistancef[3] = { 1.0f, 0.0f, 1.0f };
ImColor coldistance = ImColor(coldistancef[0], coldistancef[1], coldistancef[2]);

float colnamesf[3] = { 1.0f, 0.0f, 1.0f };
ImColor colnames = ImColor(colnamesf[0], colnamesf[1], colnamesf[2]);

char fpsinfo2[64];

PlayerCache closestCache{};
float closestmDistance = FLT_MAX;
float closestDistance = FLT_MAX;
DWORD_PTR closestMesh = NULL;
bool closestVisible = false;
DWORD_PTR closestPlayer = NULL;
Vector3 closestHead = Vector3();
Vector3 closestHead3 = Vector3();
Vector3 closestTopBox = Vector3();
Vector3 closestBottom = Vector3();
Vector3 closestVelocity = Vector3();


struct close {
	PlayerCache closestCache{};
	float closestDistance = FLT_MAX;
	float closestmDistance = FLT_MAX;
	DWORD_PTR closestMesh = NULL;
	bool closestVisible = false;
	DWORD_PTR closestPlayer = NULL;
	Vector3 closestHead = Vector3();
	Vector3 closestHead3 = Vector3();
	Vector3 closestTopBox = Vector3();
	Vector3 closestBottom = Vector3();
	Vector3 closestVelocity = Vector3();
};

close closest;

bool someoneOnScope = false;

void AsyncGetPlayerName(uintptr_t PlayerState, std::function<void(std::string)> callback) {
	std::string playerName = GetPlayerName(PlayerState);
	callback(playerName);
}

std::deque<std::pair<uintptr_t, std::string>> playerNamesDeque;

std::unordered_map<uintptr_t, std::string> playerNameMap;

void iLoop()
{
	while (!started) {
		Sleep(1000);
	}
	while (!die) {
		Sleep(1000);

		uintptr_t tempArray = point::ULevelArray;
		uint32_t tempNLevels = point::ULevels;

		if (!SETTINGS::lootEsp || !point::Uworld || !point::GameInstance || !point::LocalPlayers || !point::LocalPlayer || !point::PersistentLevel || !point::GameState || !tempArray || !tempNLevels) {
			continue;
		}

		std::unique_ptr<uintptr_t[]> ULevelArray = readArray(tempArray, tempNLevels);
		std::vector<uintptr_t> actorArrays;
		actorArrays.resize(tempNLevels);
		std::vector<uint32_t> actorCounts;
		actorCounts.resize(tempNLevels);

		VMMDLL_SCATTER_HANDLE scatterReadHandle1 = TargetProcess->CreateScatterHandle(0);

		for (int il = 0; il < tempNLevels; il++) {

			TargetProcess->QueueScatterReadEx(scatterReadHandle1, ULevelArray[il] + offsets::AActorArray, &actorArrays[il], sizeof(uintptr_t));
			TargetProcess->QueueScatterReadEx(scatterReadHandle1, ULevelArray[il] + offsets::AActorCount, &actorCounts[il], sizeof(uint32_t));

		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle1);

		std::vector<uintptr_t> actors;

		uint32_t totalActors = std::accumulate(actorCounts.begin(), actorCounts.end(), 0);

		actors.resize(totalActors);

		VMMDLL_SCATTER_HANDLE scatterReadHandle2 = TargetProcess->CreateScatterHandle(0);

		int actorIndex = 0;

		for (int il = 0; il < tempNLevels; il++) {

			for (int ic = 0; ic < actorCounts[il]; ic++) {
				TargetProcess->QueueScatterReadEx(scatterReadHandle2, actorArrays[il] + (ic * 8), &actors[actorIndex], sizeof(uintptr_t));

				actorIndex++;
			}

		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle2);

		VMMDLL_SCATTER_HANDLE scatterReadHandle3 = TargetProcess->CreateScatterHandle(0);

		for (auto it = newItemCache.begin(); it != newItemCache.end();) {

			bool found = false;
			if (it->second.useCount < 5000) {
				for (int i = 0; i < totalActors; i++) {
					if (it->first == actors[i]) {
						it->second.useCount++;
						found = true;
						if (!it->second.isLoot) continue;

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.rootComponent + 0x120, &it->second.Location, sizeof(Vector3));
						break;
					}
				}
			}

			if (!found) {
				it = newItemCache.erase(it);
			}
			else {
				++it;
			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle3);

		std::vector<Item> newItems;

		newItems.resize(totalActors);

		VMMDLL_SCATTER_HANDLE scatterReadHandle4 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < totalActors; i++)
		{
			uintptr_t actor = actors[i];

			auto it = newItemCache.find(actor);
			if (it == newItemCache.end()) {
				newItems[i].cached = false;
				newItems[i].actor = actor;
				
				TargetProcess->QueueScatterReadEx(scatterReadHandle4, newItems[i].actor + offsets::FNameKey, &newItems[i].FNameKey, sizeof(uint32_t));
			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle4);

		for (int i = 0; i < totalActors; i++)
		{
			if (!newItems[i].cached)
			{

				newItems[i].name = GetNameFromFName(newItems[i].FNameKey);

				if (newItems[i].name == ("FortPickupAthena"))
					newItems[i].isLoot = true;

			}
		}

		VMMDLL_SCATTER_HANDLE scatterReadHandle5 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < totalActors; i++)
		{
			if (newItems[i].isLoot)
			{

				TargetProcess->QueueScatterReadEx(scatterReadHandle5, newItems[i].actor + 0x198, &newItems[i].rootComponent, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle5, newItems[i].actor + offsets::ItemDefinition, &newItems[i].itemDefinition, sizeof(uintptr_t));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle5);

		VMMDLL_SCATTER_HANDLE scatterReadHandle6 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < totalActors; i++)
		{
			if (newItems[i].isLoot)
			{

				TargetProcess->QueueScatterReadEx(scatterReadHandle6, newItems[i].itemDefinition + offsets::TextPtr, &newItems[i].textPtr, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, newItems[i].rootComponent + 0x120, &newItems[i].Location, sizeof(Vector3));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle6);

		VMMDLL_SCATTER_HANDLE scatterReadHandle7 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < totalActors; i++)
		{
			if (newItems[i].isLoot)
			{

				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newItems[i].textPtr + 0x28, &newItems[i].ftext_data, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newItems[i].textPtr + 0x30, &newItems[i].ftext_length, sizeof(uint32_t));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle7);

		VMMDLL_SCATTER_HANDLE scatterReadHandle8 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < totalActors; i++)
		{
			if (newItems[i].isLoot)
			{
				if (newItems[i].ftext_length > 0 && newItems[i].ftext_length < 50)
					TargetProcess->QueueScatterReadEx(scatterReadHandle8, newItems[i].ftext_data, &newItems[i].ftext_buf, newItems[i].ftext_length * sizeof(wchar_t));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle8);

		for (int i = 0; i < totalActors; i++)
		{
			if (newItems[i].isLoot)
			{
				std::wstring wstr_buf(newItems[i].ftext_buf);

				newItems[i].itemName = std::string(wstr_buf.begin(), wstr_buf.end());

				newItemCache[newItems[i].actor] = newItems[i];

			}
		}

		readyItemCache = newItemCache;
	}
}

void pLoop()
{
	while (!started)
	{
		Sleep(1000);
	}
	while (!die) {
		if (SETTINGS::fpsCounter) {
			sprintf(fpsinfo2, ("internal FPS: %03d"), Util::get_fps2());
		}
		// Start the timer
		//auto start = std::chrono::high_resolution_clock::now();

		uintptr_t tempArray = point::PlayerArray;
		uint32_t tempNPlayers = g_nPlayers;

		if (!point::Uworld || !point::GameInstance || !point::LocalPlayers || !point::LocalPlayer || !point::PersistentLevel || !point::GameState || !tempArray || !tempNPlayers) {
			continue;
		}

		std::unique_ptr<uintptr_t[]> PlayerStateArray = readArray(tempArray, tempNPlayers);

		VMMDLL_SCATTER_HANDLE scatterReadHandle3 = TargetProcess->CreateScatterHandle(0);

		for (auto it = newCache.begin(); it != newCache.end();) {

			bool found = false;
			if (it->second.useCount < 5000) {
				for (int i = 0; i < tempNPlayers; i++) {
					if (it->first == PlayerStateArray[i]) {

						if (!it->second.Player) continue;
						if (!it->second.Mesh) continue;

						uintptr_t BoneArray = it->second.BoneArray1;
						if (!BoneArray) BoneArray = it->second.BoneArray2;

						if (!BoneArray) continue;

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Mesh + offsets::BoneArray, &it->second.BoneArray1, sizeof(uintptr_t));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Mesh + offsets::BoneArray + 0x10, &it->second.BoneArray2, sizeof(uintptr_t));

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (68 * 0x60), &it->second.HeadBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (0 * 0x60), &it->second.BottomBone, sizeof(FTransform));

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (66 * 0x60), &it->second.NeckBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (2 * 0x60), &it->second.HipBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (9 * 0x60), &it->second.UpperArmLeftBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (38 * 0x60), &it->second.UpperArmRightBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (10 * 0x60), &it->second.LeftHandBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (39 * 0x60), &it->second.RightHandBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (11 * 0x60), &it->second.LeftHand1Bone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (40 * 0x60), &it->second.RightHand1Bone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (78 * 0x60), &it->second.RightThighBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (71 * 0x60), &it->second.LeftThighBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (79 * 0x60), &it->second.RightCalfBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (72 * 0x60), &it->second.LeftCalfBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (75 * 0x60), &it->second.LeftFootBone, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, BoneArray + (82 * 0x60), &it->second.RightFootBone, sizeof(FTransform));

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.RootComponent + offsets::Velocity, &it->second.Velocity, sizeof(Vector3));

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Player + offsets::isDying, &it->second.isDying, sizeof(BYTE));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Player + offsets::IsDBNO, &it->second.isDBNO, sizeof(BYTE));

						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Mesh + offsets::ComponetToWorld, &it->second.component_to_world, sizeof(FTransform));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Mesh + offsets::LastSubmitTime, &it->second.last_submit, sizeof(float));
						TargetProcess->QueueScatterReadEx(scatterReadHandle3, it->second.Mesh + offsets::LastRenderTimeOnScreen, &it->second.last_render, sizeof(float));

						if (SETTINGS::nameEsp)
						{
							if (!it->second.gettingName && !it->second.gotName)
							{
								auto it2 = playerNameMap.find(it->second.PlayerState);
								if (it2 != playerNameMap.end()) {
									it->second.name = it2->second;
									it->second.gotName = true;
								}
								else {
									AsyncGetPlayerName(it->second.PlayerState, [&](std::string playerName) {
										it->second.name = playerName;
										it->second.gotName = true;
										playerNameMap[it->second.PlayerState] = playerName;

										playerNamesDeque.push_back({ it->second.PlayerState, playerName });

										if (playerNamesDeque.size() > 100) {
											playerNameMap.erase(playerNamesDeque.front().first);
											playerNamesDeque.pop_front();
										}
										});
								}
							}
						}

						it->second.useCount++;
						found = true;

						break;
					}
				}
			}

			if (!found) {
				it = newCache.erase(it);
			}
			else {
				++it;
			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle3);

		VMMDLL_SCATTER_HANDLE scatterReadHandle4 = TargetProcess->CreateScatterHandle(0);

		std::vector<PlayerCache> newPlayers(tempNPlayers);

		for (int i = 0; i < tempNPlayers; i++)
		{
			uintptr_t PlayerState = PlayerStateArray[i];
			if (!PlayerState) continue;

			auto it = newCache.find(PlayerState);
			if (it == newCache.end()) {
				newPlayers[i].cached = false;
				newPlayers[i].PlayerState = PlayerState;
				TargetProcess->QueueScatterReadEx(scatterReadHandle4, PlayerState + offsets::PawnPrivate, &newPlayers[i].Player, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle4, PlayerState + offsets::TeamId, &newPlayers[i].TeamId, sizeof(uint32_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle4, PlayerState + offsets::bIsABot, &newPlayers[i].isBot, sizeof(BYTE));
			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle4);

		VMMDLL_SCATTER_HANDLE scatterReadHandle5 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < tempNPlayers; i++)
		{
			if (!newPlayers[i].cached)
			{
				if (!newPlayers[i].Player) continue;

				TargetProcess->QueueScatterReadEx(scatterReadHandle5, newPlayers[i].Player + offsets::Mesh, &newPlayers[i].Mesh, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle5, newPlayers[i].Player + offsets::RootComponent, &newPlayers[i].RootComponent, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle5, newPlayers[i].Player + offsets::isDying, &newPlayers[i].isDying, sizeof(BYTE));
				TargetProcess->QueueScatterReadEx(scatterReadHandle5, newPlayers[i].Player + offsets::IsDBNO, &newPlayers[i].isDBNO, sizeof(BYTE));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle5);

		VMMDLL_SCATTER_HANDLE scatterReadHandle7 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < tempNPlayers; i++)
		{
			if (!newPlayers[i].cached)
			{
				if (!newPlayers[i].Player) continue;
				if (!newPlayers[i].Mesh) continue;

				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newPlayers[i].Mesh + offsets::BoneArray, &newPlayers[i].BoneArray1, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newPlayers[i].Mesh + offsets::BoneArray + 0x10, &newPlayers[i].BoneArray2, sizeof(uintptr_t));
				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newPlayers[i].Mesh + offsets::ComponetToWorld, &newPlayers[i].component_to_world, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newPlayers[i].Mesh + offsets::LastSubmitTime, &newPlayers[i].last_submit, sizeof(float));
				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newPlayers[i].Mesh + offsets::LastRenderTimeOnScreen, &newPlayers[i].last_render, sizeof(float));
				TargetProcess->QueueScatterReadEx(scatterReadHandle7, newPlayers[i].RootComponent + offsets::Velocity, &newPlayers[i].Velocity, sizeof(Vector3));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle7);

		VMMDLL_SCATTER_HANDLE scatterReadHandle6 = TargetProcess->CreateScatterHandle(0);

		for (int i = 0; i < tempNPlayers; i++)
		{
			if (!newPlayers[i].cached)
			{
				if (!newPlayers[i].Player) continue;
				if (!newPlayers[i].Mesh) continue;

				uintptr_t BoneArray = newPlayers[i].BoneArray1;
				if (!BoneArray) BoneArray = newPlayers[i].BoneArray2;

				if (!BoneArray) continue;

				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (68 * 0x60), &newPlayers[i].HeadBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (0 * 0x60), &newPlayers[i].BottomBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (66 * 0x60), &newPlayers[i].NeckBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (2 * 0x60), &newPlayers[i].HipBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (9 * 0x60), &newPlayers[i].UpperArmLeftBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (38 * 0x60), &newPlayers[i].UpperArmRightBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (10 * 0x60), &newPlayers[i].LeftHandBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (39 * 0x60), &newPlayers[i].RightHandBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (11 * 0x60), &newPlayers[i].LeftHand1Bone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (40 * 0x60), &newPlayers[i].RightHand1Bone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (78 * 0x60), &newPlayers[i].RightThighBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (71 * 0x60), &newPlayers[i].LeftThighBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (79 * 0x60), &newPlayers[i].RightCalfBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (72 * 0x60), &newPlayers[i].LeftCalfBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (75 * 0x60), &newPlayers[i].LeftFootBone, sizeof(FTransform));
				TargetProcess->QueueScatterReadEx(scatterReadHandle6, BoneArray + (82 * 0x60), &newPlayers[i].RightFootBone, sizeof(FTransform));

			}
		}

		TargetProcess->ExecuteScatterRead(scatterReadHandle6);

		for (int i = 0; i < tempNPlayers; i++)
		{
			if (!newPlayers[i].cached)
			{

				newPlayers[i].cached = true;
				newCache[newPlayers[i].PlayerState] = newPlayers[i];
			}
		}
		readyCache = newCache;
	}
}

inline std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}

inline std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

inline void DrawString(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[512] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImColor(color), text.c_str());
}

void Normalbox(float X, float Y, float W, float H, const ImU32& color, float rounding, int thickness) {

	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2(X, Y),
		ImVec2(X + W, Y + H),
		ImGui::GetColorU32(color),
		rounding,
		ImDrawCornerFlags_All,
		thickness
	);
}

int visibleCount = 0;
int nearCount = 0;

void render()
{
	ImVec2 midPoint = ImVec2{ (float)(Width / 2), (float)(Height / 2) };
	if (SETTINGS::show_fov) ImGui::GetBackgroundDrawList()->AddCircle(midPoint, SETTINGS::fov, COLORS::cFov, 1000, 1.f);

	if (SETTINGS::radar)
		DrawRadarHUD(SETTINGS::radarXPos, SETTINGS::radarYPos, SETTINGS::radarSize, SETTINGS::radarSize);

	bool someoneOnScope1 = false;

	if (SETTINGS::lootEsp) {
		for (const auto& pair : readyItemCache) {
			if (!pair.second.isLoot)
				continue;
			double mDistance = mainCamera.Location.Distance(pair.second.Location) / 100.f;
			Vector3 screenLocation = ProjectWorldToScreen(pair.second.Location);
			ImVec2 nameSize = ImGui::CalcTextSize(pair.second.itemName.c_str());
			float textWidth = nameSize.x;
			float xPos = (Width - textWidth) / 2;
			char dist[64];
			sprintf_s(dist, ("[%.fm]"), mDistance);
			DrawString(18, (screenLocation.x - (textWidth / 2)), screenLocation.y, COLORS::white, true, false, pair.second.itemName.c_str());
			DrawString(18, (screenLocation.x - (textWidth / 2)), screenLocation.y + nameSize.y, COLORS::white, true, false, dist);
		}
	}

	int visiblePlayers = 0;
	int nearPlayers = 0;

	for (const auto& pair : readyCache) {

		uintptr_t Player = pair.second.Player;

		uintptr_t Mesh = pair.second.Mesh;

		uint32_t teamIndex = pair.second.TeamId;

		//printf("last_submit: %f\n", pair.second.last_submit);
		//printf("last_render: %f\n", pair.second.last_render);

		bool isvis = pair.second.last_render + 0.06f >= pair.second.last_submit;

		bool isbot = pair.second.isBot & (1 << 3);

		if (!Player) continue;

		if (!Mesh) continue;

		if (pair.second.isDying & 0x10) continue;

		if (SETTINGS::ignoreKnocked) if (pair.second.isDBNO & 0x10) continue;

		if (SETTINGS::ignoreBot) if (isbot) continue;

		if (!SETTINGS::selfEsp && point::Player) if (Player == point::Player) continue;

		if (point::Player && SETTINGS::teamCheck) {
			if (teamIndex == g_localTeam && !(SETTINGS::selfEsp && Player == point::Player)) continue;
		}

		if (isvis) visiblePlayers++;
		nearPlayers++;

		FTransform HeadBone = pair.second.HeadBone;
		FTransform BottomBone = pair.second.BottomBone;
		FTransform component_to_world = pair.second.component_to_world;

		Vector3 Head3D = doMatrix(HeadBone, component_to_world);
		Vector3 Bottom3D = doMatrix(BottomBone, component_to_world);

		Vector3 Head2D = ProjectWorldToScreen(Head3D);
		Vector3 Bottom2D = ProjectWorldToScreen(Bottom3D);

		double mDistance = mainCamera.Location.Distance(Head3D) / 100.f;

		ImColor radarPointColor = ImColor(255, 0, 0, 255);

		if (isvis)
			radarPointColor = ImColor(0, 255, 0, 255);

		if (isbot)
			radarPointColor = ImColor(0, 0, 255, 255);

		if (SETTINGS::radar)
			DrawRadarPoint(Head3D, mDistance, mainCamera.Location, SETTINGS::radarScale, SETTINGS::radarZoom, SETTINGS::distanceRadar, mainCamera.Rotation.y, SETTINGS::radarXPos, SETTINGS::radarYPos, SETTINGS::radarSize, SETTINGS::radarSize, radarPointColor);

		bool inScreen = IsInScreen(Head2D, 250);

		if (SETTINGS::nameEsp && pair.second.gotName && inScreen)
		{
			char name[64];
			sprintf_s(name, "%s", pair.second.name.c_str());
			DrawString(15, Head2D.x + SETTINGS::nameposition, ProjectWorldToScreen(Vector3(Head3D.x, Head3D.y, Head3D.z + 20)).y, colnames, true, false, name);
		}

		if (SETTINGS::distance) {
			std::string DistanceText = ("[") + std::to_string(int(mDistance)) + ("m]");
			ImVec2 DistanceTextSize = ImGui::CalcTextSize(DistanceText.c_str());
			OutlinedText(Bottom2D.x - DistanceTextSize.x / 2, Bottom2D.y, coldistance, DistanceText.c_str());
		}

		if (isvis)
		{
			col2d = COLORS::cNormalBoxVisible;
			colSK = COLORS::cSkeletonVisible;
			collines = COLORS::cLinesVisible;

			coldistance = COLORS::cDistanceVisible;
			colnames = COLORS::cNamesVisible;
		}
		else {
			col2d = COLORS::cNormalBoxNotVisible;
			colSK = COLORS::cSkeletonNotVisible;
			collines = COLORS::cLinesNotVisible;

			coldistance = COLORS::cDistanceNotVisible;
			colnames = COLORS::cNamesNotVisible;
		}

		if (SETTINGS::cornerbox && inScreen) {
			Vector3 TopBox2D = ProjectWorldToScreen(Vector3(Head3D.x, Head3D.y, Head3D.z + 15));
			float CornerHeight = (float)abs(TopBox2D.y - Bottom2D.y);
			float thicknessMultiplier = SETTINGS::thicknessMultiplier;
			float widthMultiplier = SETTINGS::widthMultiplier;
			float CornerWidth = CornerHeight * widthMultiplier;
			CornerWidth *= thicknessMultiplier;
			float thickness = SETTINGS::thickness_two;
			for (int i = 0; i < thickness; ++i) {
				CornerBox(TopBox2D.x - (CornerWidth / 2) - i, TopBox2D.y - i, CornerWidth + 2 * i, CornerHeight + 2 * i, col2d, 0);
			}
		}

		if (SETTINGS::normalBox && inScreen) {
			Vector3 TopBox2D = ProjectWorldToScreen(Vector3(Head3D.x, Head3D.y, Head3D.z + 15));

			float CornerHeight = (float)abs(TopBox2D.y - Bottom2D.y);
			float CornerWidth = CornerHeight * 0.60;

			Normalbox(TopBox2D.x - (CornerWidth / 2), (TopBox2D.y), CornerWidth, CornerHeight, col2d, 0, 2);
		}

		if (SETTINGS::skeleton && inScreen) {
			float tk = SETTINGS::skeleton_thickness;

			Vector3 vHip = doMatrix(pair.second.HipBone, component_to_world);
			Vector3 vNeck = doMatrix(pair.second.NeckBone, component_to_world);
			Vector3 vUpperArmLeft = doMatrix(pair.second.UpperArmLeftBone, component_to_world);
			Vector3 vUpperArmRight = doMatrix(pair.second.UpperArmRightBone, component_to_world);
			Vector3 vLeftHand = doMatrix(pair.second.LeftHandBone, component_to_world);
			Vector3 vRightHand = doMatrix(pair.second.RightHandBone, component_to_world);
			Vector3 vLeftHand1 = doMatrix(pair.second.LeftHand1Bone, component_to_world);
			Vector3 vRightHand1 = doMatrix(pair.second.RightHand1Bone, component_to_world);
			Vector3 vRightThigh = doMatrix(pair.second.RightThighBone, component_to_world);
			Vector3 vLeftThigh = doMatrix(pair.second.LeftThighBone, component_to_world);
			Vector3 vRightCalf = doMatrix(pair.second.RightCalfBone, component_to_world);
			Vector3 vLeftCalf = doMatrix(pair.second.LeftCalfBone, component_to_world);
			Vector3 vLeftFoot = doMatrix(pair.second.LeftFootBone, component_to_world);
			Vector3 vRightFoot = doMatrix(pair.second.RightFootBone, component_to_world);

			Vector3 vHeadBoneOut = Head2D;
			Vector3 vHipOut = ProjectWorldToScreen(vHip);
			Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
			Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
			Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
			Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
			Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
			Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
			Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
			Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
			Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
			Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
			Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
			Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
			Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);

			if (SETTINGS::headBone)
			{

				float radiusbrute = abs(vHeadBoneOut.y - vNeckOut.y);
				float radius = radiusbrute / 2;

				DrawCircle(vHeadBoneOut.x, vHeadBoneOut.y, radius, colSK, 120, SETTINGS::skeleton_thickness);

			}

			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vHeadBoneOut.x, vHeadBoneOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), colSK, tk);
		}

		if (SETTINGS::lines)
		{
			switch (SETTINGS::linesStart)
			{
				case 0:
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ (float)(Width / 2), (float)(Height) }, ImVec2{ (float)(Bottom2D.x), (float)(Bottom2D.y) }, collines, SETTINGS::snapline_thickness);
					break;
				case 1:
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ (float)(Width / 2), (float)(Height / 2) }, ImVec2{ (float)(Bottom2D.x), (float)(Bottom2D.y) }, collines, SETTINGS::snapline_thickness);
					break;
				case 2:
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ (float)(Width / 2), 0.f }, ImVec2{ (float)(Bottom2D.x), (float)(Bottom2D.y) }, collines, SETTINGS::snapline_thickness);
					break;

			}
		}

		if (inScreen && isvis)
		{
			float distance = sqrt(pow(Head2D.x - midPoint.x, 2) + pow(Head2D.y - midPoint.y, 2));
			if ((distance <= SETTINGS::fov) && (distance < closestDistance)) {
				someoneOnScope1 = true;
				closestmDistance = g_localPos.Distance(Head3D);
				closestDistance = distance;
				closestMesh = Mesh;
				closestHead = Head2D;
				closestHead3 = Head3D;
				closestVisible = isvis;
				closestPlayer = Player;
				closestVelocity = pair.second.Velocity;
				closestCache = pair.second;
			}
		}
	}

	visibleCount = visiblePlayers;
	nearCount = nearPlayers;

	close closer;

	closer.closestmDistance = closestmDistance;
	closer.closestDistance = closestDistance;
	closer.closestMesh = closestMesh;
	closer.closestHead = closestHead;
	closer.closestHead3 = closestHead3;
	closer.closestVisible = closestVisible;
	closer.closestPlayer = closestPlayer;
	closer.closestVelocity = closestVelocity;
	closer.closestCache = closestCache;

	closestDistance = FLT_MAX;
	closestVisible = false;

	closest = closer;
	someoneOnScope = someoneOnScope1;

	if (SETTINGS::aimline && SETTINGS::KmboxAim && point::Player && SETTINGS::visCheck && closer.closestVisible && closer.closestPlayer != point::Player && point::Player) {
		switch (SETTINGS::aimBone)
		{
		case 0: {
			ImGui::GetBackgroundDrawList()->AddLine(midPoint, ImVec2(closestHead.x, closestHead.y), COLORS::cAimLines, 1.5f);
			break;
		}
		case 1: {
			Vector3 vNeck = doMatrix(closestCache.NeckBone, closestCache.component_to_world);
			Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
			ImGui::GetBackgroundDrawList()->AddLine(midPoint, ImVec2(vNeckOut.x, vNeckOut.y), COLORS::cAimLines, 1.5f);
			break;
		}
		case 2: {
			Vector3 vHip = doMatrix(closestCache.HipBone, closestCache.component_to_world);
			Vector3 vHipOut = ProjectWorldToScreen(vHip);
			ImGui::GetBackgroundDrawList()->AddLine(midPoint, ImVec2(vHipOut.x, vHipOut.y), COLORS::cAimLines, 1.5f);
			break;
		}
		case 3: {
			Vector3 screenMiddlePoint = Vector3(Width / 2, Height / 2, 0);

			Vector3 NeckBone3 = doMatrix(closestCache.NeckBone, closestCache.component_to_world);
			Vector3 NeckBone = ProjectWorldToScreen(NeckBone3);

			Vector3 HipBone3 = doMatrix(closestCache.HipBone, closestCache.component_to_world);
			Vector3 HipBone = ProjectWorldToScreen(HipBone3);

			Vector3 CenterBone3 = (NeckBone3 + HipBone3) / 2;
			Vector3 CenterBone = ProjectWorldToScreen(CenterBone3);

			ImGui::GetBackgroundDrawList()->AddLine(midPoint, ImVec2(CenterBone.x, CenterBone.y), COLORS::cAimLines, 1.5f);
			break;
		}
		}
	}
}

void aim()
{
	while (!die) {
		close target = closest;
		if (!pickaxe && point::Player && someoneOnScope)
		{
			if (SETTINGS::visCheck)
			{
				if (!target.closestVisible) target.closestMesh = 0;
			}
			if (target.closestMesh != 0 && target.closestPlayer != point::Player)
			{

				Vector3 targetBone3 = target.closestHead3;
				Vector3 targetBone = target.closestHead;

				if (SETTINGS::aimAtClosestBone) {

					Vector3 screenMiddlePoint = Vector3(Width/2, Height/2, 0);

					Vector3 NeckBone3 = doMatrix(target.closestCache.NeckBone, target.closestCache.component_to_world);
					Vector3 NeckBone = ProjectWorldToScreen(NeckBone3);

					Vector3 HipBone3 = doMatrix(target.closestCache.HipBone, target.closestCache.component_to_world);
					Vector3 HipBone = ProjectWorldToScreen(HipBone3);

					Vector3 CenterBone3 = (NeckBone3 + HipBone3) / 2;
					Vector3 CenterBone = ProjectWorldToScreen(CenterBone3);

					double distanceHead = screenMiddlePoint.GetDistance(screenMiddlePoint, targetBone3);
					double distanceNeck = screenMiddlePoint.GetDistance(screenMiddlePoint, NeckBone);
					double distanceHip = screenMiddlePoint.GetDistance(screenMiddlePoint, HipBone);
					double distanceCenter = screenMiddlePoint.GetDistance(screenMiddlePoint, CenterBone);

					// Find the closest point
					double minDistance = distanceHead;
					int closestBoneInt = 0;

					if (distanceNeck < minDistance)
					{
						minDistance = distanceNeck;
						closestBoneInt = 1;
					}

					if (distanceHip < minDistance)
					{
						minDistance = distanceHip;
						closestBoneInt = 2;
					}

					if (distanceCenter < minDistance)
					{
						minDistance = distanceCenter;
						closestBoneInt = 3;
					}

					switch (closestBoneInt) {
						case 1: {
							targetBone3 = NeckBone3;
							targetBone = NeckBone;
							break;
						}
						case 2: {
							targetBone3 = HipBone3;
							targetBone = HipBone;
							break;
						}
						case 3: {
							targetBone3 = CenterBone3;
							targetBone = CenterBone;
							break;
						}
					}
				}
				else
				{
					switch (SETTINGS::aimBone)
					{
						case 1: {
							targetBone3 = doMatrix(target.closestCache.NeckBone, target.closestCache.component_to_world);
							targetBone = ProjectWorldToScreen(targetBone3);
							break;
						}
						case 2: {
							targetBone3 = doMatrix(target.closestCache.HipBone, target.closestCache.component_to_world);
							targetBone = ProjectWorldToScreen(targetBone3);
							break;
						}
						case 3: {
							Vector3 NeckBone3 = doMatrix(target.closestCache.NeckBone, target.closestCache.component_to_world);

							Vector3 HipBone3 = doMatrix(target.closestCache.HipBone, target.closestCache.component_to_world);

							targetBone3 = (NeckBone3 + HipBone3) / 2;
							targetBone = ProjectWorldToScreen(targetBone3);
							break;
						}
					}
				}

				if (SETTINGS::prediction && ShouldUsePrediction) {
					auto result = predict_location(targetBone3, target.closestVelocity, target.closestmDistance);
					targetBone = ProjectWorldToScreen(result);
				}

				if (targetBone.x != 0 || targetBone.y != 0)
				{
					if (SETTINGS::triggerbot) {
						bool keypressed = false;

						if (SETTINGS::getKeyReady)
							keypressed = TargetProcess->IsKeyDown(SETTINGS::triggerKey);
						else{
							if (SETTINGS::kmboxnet)
								keypressed = kmNet_monitor_mouse_right();
						}

						if (keypressed) {
							if (target.closestmDistance / 100 <= SETTINGS::TriggerBotDistance) {
								if (IsShootable(LUR, targetBone3)) {

									if (has_clicked) {
										tb_begin = std::chrono::steady_clock::now();
										has_clicked = 0;
									}
									tb_end = std::chrono::steady_clock::now();
									tb_time_since = std::chrono::duration_cast<std::chrono::milliseconds>(tb_end - tb_begin).count();


									if (tb_time_since >= SETTINGS::custom_delay) {

										if (SETTINGS::kmboxnet) {
											kmNet_mouse_left(1);
											Sleep(SETTINGS::clickDelay);
											kmNet_mouse_left(0);
										}
										else if (SETTINGS::kmboxserial) {
											km_click(SETTINGS::clickDelay);
										}

										has_clicked = 1;
									}

								}
							}
						}
					}

					if (SETTINGS::KmboxAim)
					{

						bool keypressed = false;

						if (SETTINGS::getKeyReady)
							keypressed = TargetProcess->IsKeyDown(SETTINGS::key);
						else {
							if (SETTINGS::kmboxnet)
								keypressed = kmNet_monitor_mouse_right();
						}

						if (keypressed) {
							float screen_center_x = Width / 2;
							float screen_center_y = Height / 2;
							float TargetX = 0;
							float TargetY = 0;
							if (targetBone.x != 0)
							{
								if
									(targetBone.x > screen_center_x)
								{
									TargetX = -(screen_center_x - targetBone.x); if (TargetX + screen_center_x > screen_center_x * 2)TargetX = 0;
								}
								if
									(targetBone.x < screen_center_x)
								{
									TargetX = targetBone.x - screen_center_x; if (TargetX + screen_center_x < 0)TargetX = 0;
								}
							}
							if (targetBone.y != 0)
							{
								if
									(targetBone.y > screen_center_y)
								{
									TargetY = -(screen_center_y - targetBone.y); if (TargetY + screen_center_y > screen_center_y * 2) TargetY = 0;
								}
								if
									(targetBone.y < screen_center_y)
								{
									TargetY = targetBone.y - screen_center_y; if (TargetY + screen_center_y < 0) TargetY = 0;
								}
							}

							ImVec2 Angles(TargetX / SETTINGS::dev_int, TargetY / SETTINGS::dev_int);

							if (Angles.x < 1 && Angles.x > 0.25) Angles.x = 1;

							if (Angles.y < 1 && Angles.y > 0.25) Angles.y = 1;

							if (Angles.x < -0.25 && Angles.x > -1) Angles.x = -1;

							if (Angles.y < -0.25 && Angles.y > -1) Angles.y = -1;

							if (SETTINGS::kmboxnet) {
								kmNet_mouse_move_auto(Angles.x, Angles.y, SETTINGS::aimSteps);
							}
							else
								if (SETTINGS::kmboxserial) {
									km_move(Angles.x, Angles.y);
								}

							Sleep(SETTINGS::aimSteps);
						}
					}
				}
			}
		}
		//Sleep(1);
	}
}

bool bMenu = true;

const char* cheat_name = ("Asgard");
const char* game_status = ("Fortnite");
const char* developer = ("asgardian12");

const char* ping = ("7ms");
const char* world_time = ("12:20am");

std::string KeyCodeToKeyText(int keyCode) {
	switch (keyCode) {
	case VK_RETURN:
		return ("Enter");
	case VK_TAB:
		return ("Tab");
	case VK_BACK:
		return ("Backspace");
	case VK_ESCAPE:
		return ("Escape");
	case VK_SPACE:
		return ("Space");
	case VK_RBUTTON:
		return ("Right Mouse");
	case VK_LBUTTON:
		return ("Left Mouse");
	case VK_MBUTTON:
		return ("Middle Mouse");
	case VK_XBUTTON1:
		return ("X1 Mouse");
	case VK_XBUTTON2:
		return ("X2 Mouse");
	default:
		if (keyCode >= 0x30 && keyCode <= 0x5A) {
			return std::string(1, static_cast<char>(keyCode));
		}
		else {
			return std::to_string(keyCode);
		}
	}
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);
DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

void Menu() {

	if (SETTINGS::fpsCounter) {
		char fpsinfo[64];
		sprintf(fpsinfo, ("FPS: %03d"), Util::get_fps());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(0, 0), ImColor(255, 255, 255), fpsinfo);
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(0, 20), ImColor(255, 255, 255), fpsinfo2);
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(0, 40), ImColor(255, 255, 255), (("Weapon: ") + g_localPlayer.localWeaponName).c_str());

		float textWidth = ImGui::CalcTextSize((("[ visible: ") + std::to_string(visibleCount) + (" | near: ") + std::to_string(nearCount) + (" ]")).c_str()).x;

		float xPos = (Width - textWidth) / 2;

		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(xPos, 20), ImColor(255, 255, 255), (("[ visible: ") + std::to_string(visibleCount) + (" | near: ") + std::to_string(nearCount) + (" ]")).c_str());
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) bMenu = !bMenu; //0xDC "\|" //0x71 "f2" //0x5A "z"
	if (bMenu)
	{

		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(0, 0), style->WindowBorderSize = 0;
		style->ItemSpacing = ImVec2(12, 12);
		style->ScrollbarSize = 7.f;

		static float color[4] = { 1.f, 0.f, 1.f, 1.f };
		c::accent = { color[0], color[1], color[2], 1.f };

		ImGui::SetNextWindowSize({ c::bg::size });

		ImGui::Begin(("IMGUI"), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
		{
			const ImVec2& pos = ImGui::GetWindowPos();
			const ImVec2& spacing = style->ItemSpacing;
			const ImVec2& region = ImGui::GetContentRegionMax();

			ImGui::GetBackgroundDrawList()->AddRectFilled(pos, { pos + region }, ImGui::GetColorU32(c::bg::background), c::bg::rounding);

			ImGui::GetBackgroundDrawList()->AddRectFilled(pos + spacing, pos + ImVec2(155, 90), ImGui::GetColorU32(c::child::background), c::child::rounding);
			ImGui::GetBackgroundDrawList()->AddRect(pos + spacing, pos + ImVec2(155, 90), ImGui::GetColorU32(c::child::outline), c::child::rounding);

			ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, 90) + spacing, pos + ImVec2(155, region.y - spacing.y), ImGui::GetColorU32(c::child::background), c::child::rounding);
			ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(0, 90) + spacing, pos + ImVec2(155, region.y - spacing.y), ImGui::GetColorU32(c::child::outline), c::child::rounding);

			ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(155, 0) + spacing, pos + ImVec2(region.x - 90 - spacing.x, 90), ImGui::GetColorU32(c::child::background), c::child::rounding);
			ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(155, 0) + spacing, pos + ImVec2(region.x - 90 - spacing.x, 90), ImGui::GetColorU32(c::child::outline), c::child::rounding);

			ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(region.x - 90 - spacing.x, 0) + spacing, pos + ImVec2(region.x - spacing.x, 90), ImGui::GetColorU32(c::child::background), c::child::rounding);
			ImGui::GetBackgroundDrawList()->AddRect(pos + ImVec2(region.x - 90 - spacing.x, 0) + spacing, pos + ImVec2(region.x - spacing.x, 90), ImGui::GetColorU32(c::child::outline), c::child::rounding);

			ImGui::GetBackgroundDrawList()->AddText(fonts::inter_bold, 17, pos + (ImVec2(155, 90) + spacing - ImGui::CalcTextSize(("Asgard"))) / 2, ImGui::GetColorU32(c::text::text_active), ("Asgard"));

			static int tabs = 0, sub_tabs = 0;

			ImGui::SetCursorPos(ImVec2(spacing.x * 2, 90 + spacing.y * 2));
			ImGui::BeginGroup();
			{
				if (ImGui::Tabs(0 == tabs, pic::aimbot, ("Aimbot"), ImVec2(155 - spacing.x * 3, 35), NULL)) tabs = 0;

				if (ImGui::Tabs(1 == tabs, pic::visuals, ("Visuals"), ImVec2(155 - spacing.x * 3, 35), NULL)) tabs = 1;

				if (ImGui::Tabs(5 == tabs, pic::items, ("Items"), ImVec2(155 - spacing.x * 3, 35), NULL)) tabs = 5;

				if (ImGui::Tabs(4 == tabs, pic::visuals, ("Radar"), ImVec2(155 - spacing.x * 3, 35), NULL)) tabs = 4;

				if (ImGui::Tabs(2 == tabs, pic::skins, ("Colors"), ImVec2(155 - spacing.x * 3, 35), NULL)) tabs = 2;

				if (ImGui::Tabs(3 == tabs, pic::settings, ("Settings"), ImVec2(155 - spacing.x * 3, 35), NULL)) tabs = 3;
			}
			ImGui::EndGroup();

			static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

			tab_alpha = ImClamp(tab_alpha + (4.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);
			tab_add = ImClamp(tab_add + (std::round(350.f) * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);

			if (tab_alpha == 0.f && tab_add == 0.f) active_tab = tabs;

			if (active_tab == 0) {
				ImGui::SetCursorPos(ImVec2(155 + (spacing.x * 2) + (48 / 2), ((48 + spacing.y) / 2) - 1));
				ImGui::BeginGroup();
				{
					if (ImGui::SubTab(0 == sub_tabs, pic::weapon_ak47, ImVec2(48, 48))) sub_tabs = 0;
					ImGui::SameLine(0, 24);
					if (ImGui::SubTab(1 == sub_tabs, pic::weapon_m4a1, ImVec2(48, 48))) sub_tabs = 1;
					ImGui::SameLine(0, 24);
					if (ImGui::SubTab(2 == sub_tabs, pic::weapon_galil, ImVec2(48, 48))) sub_tabs = 2;
					ImGui::SameLine(0, 24);
					if (ImGui::SubTab(3 == sub_tabs, pic::weapon_rifle, ImVec2(48, 48))) sub_tabs = 3;
				}
				ImGui::EndGroup();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);

			ImGui::SetCursorPos(ImVec2(155, 90) + spacing);
			//aimbot
			if (active_tab == 0) {

				if (sub_tabs == 0) { // rifle
					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("GENERAL"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 2.4f));
						{

							ImGui::Checkbox(("KMbox Aim"), &SETTINGS::rifle::KmboxAim);

							ImGui::Checkbox(("Prediction"), &SETTINGS::rifle::prediction);

							ImGui::Checkbox(("Show FOV"), &SETTINGS::rifle::show_fov);

						}
						ImGui::EndChild();

						ImGui::BeginChild(("TRIGGERBOT"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 1.7f));
						{
							ImGui::Checkbox(("TriggerBot"), &SETTINGS::rifle::triggerbot);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::rifle::triggerKey)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::rifle::triggerKey = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::SliderInt(("Max distance"), &SETTINGS::rifle::TriggerBotDistance, 0, 300);

							ImGui::SliderInt(("Delay"), &SETTINGS::rifle::custom_delay, 0, 50);

							ImGui::SliderInt(("Click Delay"), &SETTINGS::rifle::clickDelay, 1, 50);


						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("CONFIGURE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
						{

							ImGui::SliderFloat(("Aim FOV Size"), &SETTINGS::rifle::fov, 0.f, 1000.f, ("%.1f"));

							ImGui::SliderInt(("Aim Steps"), &SETTINGS::rifle::aimSteps, 0.f, 10.f);

							ImGui::SliderInt(("Aim Smooth"), &SETTINGS::rifle::dev_int, 1.f, 20.f);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::rifle::key)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::rifle::key = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::Checkbox(("Closest Bone"), &SETTINGS::rifle::aimAtClosestBone);

							if (!SETTINGS::rifle::aimAtClosestBone) {
								ImGui::Text("Aim Bone");
								ImGui::SameLine();
								float comboWidth = 400.0f;
								ImGui::SetNextItemWidth(comboWidth);

								const char* items[] = { "Head", "Neck", "Body" };
								int currentItem = SETTINGS::rifle::aimBone;

								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + -75.0f);

								if (ImGui::BeginCombo("Aim Bone", items[currentItem], ImGuiComboFlags_HeightRegular)) {
									for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
										bool isSelected = (currentItem == i);
										if (ImGui::Selectable(items[i], isSelected))
											SETTINGS::rifle::aimBone = i;
										if (isSelected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}
							}

							ImGui::SetCursorPos(ImVec2(15, 354));
							ImGui::ColorEdit4(("Aimline"), &COLORS::cAimLines.Value.x, picker_flags);

							ImGui::SetCursorPos(ImVec2(15, 384));
							ImGui::ColorEdit4(("Fov"), &COLORS::cFov.Value.x, picker_flags);

						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
				}

				if (sub_tabs == 1) { // shotgun
					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("GENERAL"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 2.4f));
						{

							ImGui::Checkbox(("KMbox Aim"), &SETTINGS::shotgun::KmboxAim);

							ImGui::Checkbox(("Prediction"), &SETTINGS::shotgun::prediction);

							ImGui::Checkbox(("Show FOV"), &SETTINGS::shotgun::show_fov);

						}
						ImGui::EndChild();

						ImGui::BeginChild(("TRIGGERBOT"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 1.7f));
						{
							ImGui::Checkbox(("TriggerBot"), &SETTINGS::shotgun::triggerbot);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::shotgun::triggerKey)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::shotgun::triggerKey = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::SliderInt(("Max distance"), &SETTINGS::shotgun::TriggerBotDistance, 0, 300);

							ImGui::SliderInt(("Delay"), &SETTINGS::shotgun::custom_delay, 0, 50);

							ImGui::SliderInt(("Click Delay"), &SETTINGS::shotgun::clickDelay, 1, 50);


						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("CONFIGURE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
						{

							ImGui::SliderFloat(("Aim FOV Size"), &SETTINGS::shotgun::fov, 0.f, 1000.f, ("%.1f"));

							ImGui::SliderInt(("Aim Steps"), &SETTINGS::shotgun::aimSteps, 0.f, 10.f);

							ImGui::SliderInt(("Aim Smooth"), &SETTINGS::shotgun::dev_int, 1.f, 20.f);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::shotgun::key)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::shotgun::key = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::Checkbox(("Closest Bone"), &SETTINGS::shotgun::aimAtClosestBone);

							if (!SETTINGS::shotgun::aimAtClosestBone) {
								ImGui::Text("Aim Bone");
								ImGui::SameLine();
								float comboWidth = 400.0f;
								ImGui::SetNextItemWidth(comboWidth);

								const char* items[] = { "Head", "Neck", "Body" };
								int currentItem = SETTINGS::shotgun::aimBone;

								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + -75.0f);

								if (ImGui::BeginCombo("Aim Bone", items[currentItem], ImGuiComboFlags_HeightRegular)) {
									for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
										bool isSelected = (currentItem == i);
										if (ImGui::Selectable(items[i], isSelected))
											SETTINGS::shotgun::aimBone = i;
										if (isSelected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}
							}

							ImGui::SetCursorPos(ImVec2(15, 354));
							ImGui::ColorEdit4(("Aimline"), &COLORS::cAimLines.Value.x, picker_flags);

							ImGui::SetCursorPos(ImVec2(15, 384));
							ImGui::ColorEdit4(("Fov"), &COLORS::cFov.Value.x, picker_flags);

						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
				}
				
				if (sub_tabs == 2) { // smg
					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("GENERAL"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 2.4f));
						{

							ImGui::Checkbox(("KMbox Aim"), &SETTINGS::smg::KmboxAim);

							ImGui::Checkbox(("Prediction"), &SETTINGS::smg::prediction);

							ImGui::Checkbox(("Show FOV"), &SETTINGS::smg::show_fov);

						}
						ImGui::EndChild();

						ImGui::BeginChild(("TRIGGERBOT"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 1.7f));
						{
							ImGui::Checkbox(("TriggerBot"), &SETTINGS::smg::triggerbot);
							
							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::smg::triggerKey)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::smg::triggerKey = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::SliderInt(("Max distance"), &SETTINGS::smg::TriggerBotDistance, 0, 300);

							ImGui::SliderInt(("Delay"), &SETTINGS::smg::custom_delay, 0, 50);

							ImGui::SliderInt(("Click Delay"), &SETTINGS::smg::clickDelay, 1, 50);


						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("CONFIGURE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
						{

							ImGui::SliderFloat(("Aim FOV Size"), &SETTINGS::smg::fov, 0.f, 1000.f, ("%.1f"));

							ImGui::SliderInt(("Aim Steps"), &SETTINGS::smg::aimSteps, 0.f, 10.f);

							ImGui::SliderInt(("Aim Smooth"), &SETTINGS::smg::dev_int, 1.f, 20.f);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::smg::key)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::smg::key = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::Checkbox(("Closest Bone"), &SETTINGS::smg::aimAtClosestBone);

							if (!SETTINGS::smg::aimAtClosestBone) {
								ImGui::Text("Aim Bone");
								ImGui::SameLine();
								float comboWidth = 400.0f;
								ImGui::SetNextItemWidth(comboWidth);

								const char* items[] = { "Head", "Neck", "Body" };
								int currentItem = SETTINGS::smg::aimBone;

								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + -75.0f);

								if (ImGui::BeginCombo("Aim Bone", items[currentItem], ImGuiComboFlags_HeightRegular)) {
									for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
										bool isSelected = (currentItem == i);
										if (ImGui::Selectable(items[i], isSelected))
											SETTINGS::smg::aimBone = i;
										if (isSelected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}
							}

							ImGui::SetCursorPos(ImVec2(15, 354));
							ImGui::ColorEdit4(("Aimline"), &COLORS::cAimLines.Value.x, picker_flags);

							ImGui::SetCursorPos(ImVec2(15, 384));
							ImGui::ColorEdit4(("Fov"), &COLORS::cFov.Value.x, picker_flags);

						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
				}

				if (sub_tabs == 3) { // sniper
					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("GENERAL"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 2.4f));
						{

							ImGui::Checkbox(("KMbox Aim"), &SETTINGS::sniper::KmboxAim);

							ImGui::Checkbox(("Prediction"), &SETTINGS::sniper::prediction);

							ImGui::Checkbox(("Show FOV"), &SETTINGS::sniper::show_fov);

						}
						ImGui::EndChild();

						ImGui::BeginChild(("TRIGGERBOT"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 1.7f));
						{
							ImGui::Checkbox(("TriggerBot"), &SETTINGS::sniper::triggerbot);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::sniper::triggerKey)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::sniper::triggerKey = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::SliderInt(("Max distance"), &SETTINGS::sniper::TriggerBotDistance, 0, 300);

							ImGui::SliderInt(("Delay"), &SETTINGS::sniper::custom_delay, 0, 50);

							ImGui::SliderInt(("Click Delay"), &SETTINGS::sniper::clickDelay, 1, 50);


						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					{
						ImGui::BeginChild(("CONFIGURE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
						{

							ImGui::SliderFloat(("Aim FOV Size"), &SETTINGS::sniper::fov, 0.f, 1000.f, ("%.1f"));

							ImGui::SliderInt(("Aim Steps"), &SETTINGS::sniper::aimSteps, 0.f, 10.f);

							ImGui::SliderInt(("Aim Smooth"), &SETTINGS::sniper::dev_int, 1.f, 20.f);

							if (ImGui::Button((("Keybind : ") + KeyCodeToKeyText(SETTINGS::sniper::key)).c_str(), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 20)))
							{
								bool keySelected = false;
								while (!keySelected)
								{
									for (int i = 0; i <= 0xA5; i++) {
										if (GetAsyncKeyState(i) & 1) {
											SETTINGS::sniper::key = i;
											keySelected = true;
										}
									}
								}
							}

							ImGui::Checkbox(("Closest Bone"), &SETTINGS::sniper::aimAtClosestBone);

							if (!SETTINGS::sniper::aimAtClosestBone) {
								ImGui::Text("Aim Bone");
								ImGui::SameLine();
								float comboWidth = 400.0f;
								ImGui::SetNextItemWidth(comboWidth);

								const char* items[] = { "Head", "Neck", "Body" };
								int currentItem = SETTINGS::sniper::aimBone;

								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + -75.0f);

								if (ImGui::BeginCombo("Aim Bone", items[currentItem], ImGuiComboFlags_HeightRegular)) {
									for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
										bool isSelected = (currentItem == i);
										if (ImGui::Selectable(items[i], isSelected))
											SETTINGS::sniper::aimBone = i;
										if (isSelected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}
							}

							ImGui::SetCursorPos(ImVec2(15, 354));
							ImGui::ColorEdit4(("Aimline"), &COLORS::cAimLines.Value.x, picker_flags);

							ImGui::SetCursorPos(ImVec2(15, 384));
							ImGui::ColorEdit4(("Fov"), &COLORS::cFov.Value.x, picker_flags);

						}
						ImGui::EndChild();
					}
					ImGui::EndGroup();
				}
			}
			//visuals
			if (active_tab == 1) {

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("PLAYER ESP"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						ImGui::Checkbox(("2D Box"), &SETTINGS::normalBox);

						ImGui::Checkbox(("Corner Box"), &SETTINGS::cornerbox);

						ImGui::Checkbox(("Skeleton"), &SETTINGS::skeleton);

						ImGui::Checkbox(("HeadBone"), &SETTINGS::headBone);

						ImGui::Checkbox(("Draw Aimline"), &SETTINGS::aimline);

						ImGui::Checkbox(("Snaplines"), &SETTINGS::lines);

						ImGui::Checkbox(("Distance"), &SETTINGS::distance);

						ImGui::Checkbox(("Name"), &SETTINGS::nameEsp);

						ImGui::Checkbox(("Ignore Bots"), &SETTINGS::ignoreBot);

						ImGui::Checkbox(("Ignore Knocked"), &SETTINGS::ignoreKnocked);
					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("CUSTOMIZE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						if (SETTINGS::skeleton)
						{
							ImGui::SliderFloat(("Skeleton Thickness"), &SETTINGS::skeleton_thickness, 1, 10);

						}

						if (SETTINGS::lines) {

							ImGui::SliderFloat(("Snapline Thickness"), &SETTINGS::snapline_thickness, 1, 10);

							ImGui::Dummy(ImVec2(0.0f, 5.0f));

							ImGui::Text("Snaplines Start");
							ImGui::SameLine();
							float comboWidth = 400.0f; 
							ImGui::SetNextItemWidth(comboWidth);

							const char* items[] = { "Bottom", "Center", "Top" };
							int currentItem = SETTINGS::linesStart;
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + -110.0f); 

							if (ImGui::BeginCombo("Snaplines Start", items[currentItem], ImGuiComboFlags_HeightRegular)) {
								for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
									bool isSelected = (currentItem == i);
									if (ImGui::Selectable(items[i], isSelected))
										SETTINGS::linesStart = i;
									if (isSelected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}
						}




					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
			//item
			if (active_tab == 5) //items
			{
				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("ITEM ESP"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						ImGui::Checkbox(("Loot ESP (Testing)"), &SETTINGS::lootEsp);

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("CUSTOMIZE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
			//radar
			if (active_tab == 4) //radar
			{
				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("RADAR"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{
						ImGui::Checkbox(("Radar"), &SETTINGS::radar);

						ImGui::Checkbox(("Distance Radar"), &SETTINGS::distanceRadar);

						ImGui::SliderFloat(("Radar Size"), &SETTINGS::radarSize, 10.f, Height, "%.1f");

						ImGui::SliderFloat(("Radar Zoom"), &SETTINGS::radarZoom, 0, 2, "%.2f");

						ImGui::SliderFloat(("Radar Limit"), &SETTINGS::radarScale, 50, 500);

						ImGui::SliderInt(("Radar X"), &SETTINGS::radarXPos, 0, Width);

						ImGui::SliderInt(("Radar Y"), &SETTINGS::radarYPos, 0, Height);
					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
			//colors
			if (active_tab == 2) {

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("COLORS VISIBLE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						ImGui::ColorEdit4(("2d box"), &COLORS::cNormalBoxVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Skeleton"), &COLORS::cSkeletonVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Lines"), &COLORS::cLinesVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Distance"), &COLORS::cDistanceVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Name"), &COLORS::cNamesVisible.Value.x, picker_flags);

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("COLORS NOTVISIBLE"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{
						ImGui::ColorEdit4(("2d box"), &COLORS::cNormalBoxNotVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Skeleton"), &COLORS::cSkeletonNotVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Lines"), &COLORS::cLinesNotVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Distance"), &COLORS::cDistanceNotVisible.Value.x, picker_flags);

						ImGui::ColorEdit4(("Name"), &COLORS::cNamesNotVisible.Value.x, picker_flags);

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}
			//settings
			if (active_tab == 3) {

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("SETTINGS"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						ImGui::Checkbox(("System Info"), &SETTINGS::fpsCounter);

						ImGui::Checkbox(("vsync"), &SETTINGS::vsync);

						ImGui::Checkbox(("hdmi fuser"), &SETTINGS::hdmiFuser);

						ImGui::SetCursorPos(ImVec2(15, 384));
						ImGui::ColorEdit4(("Menu"), color, picker_flags);

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				{
					ImGui::BeginChild(("CONFIG"), ImVec2((region.x - (spacing.x * 3 + 155)) / 2, (region.y - (spacing.y * 3 + 90)) / 0.97f));
					{

						if (ImGui::Button(("Load Config"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
							Load_Settings(("C:\\FNCFG"));
							ImGui::Notification({ ImGuiToastType_Success, 4000, ("Config Loaded Successfully") });
						}

						if (ImGui::Button(("Save Config"), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40))) {
							Save_Settings(("C:\\FNCFG"));
							ImGui::Notification({ ImGuiToastType_Success, 4000, ("Config Saved Successfully") });
						}

					}
					ImGui::EndChild();
				}
				ImGui::EndGroup();
			}

			ImGui::PopStyleVar();
		}
		ImGui::End();

		//static float ibar_size = ImGui::CalcTextSize(cheat_name).x + ImGui::CalcTextSize(("|")).x + ImGui::CalcTextSize(developer).x + ImGui::CalcTextSize(("|")).x + ImGui::CalcTextSize(ping).x + ImGui::CalcTextSize(("|")).x + ImGui::CalcTextSize(world_time).x + (style->ItemSpacing.x * 8);
		//static float position = GetSystemMetrics(SM_CXSCREEN);
		//position = ImLerp(position, true ? GetSystemMetrics(SM_CXSCREEN) - (ibar_size + 7) : GetSystemMetrics(SM_CXSCREEN), ImGui::GetIO().DeltaTime * 8.f);

		//if (position <= (GetSystemMetrics(SM_CXSCREEN) - 2)) {

		//	ImGui::SetNextWindowPos(ImVec2(position, 5));
		//	ImGui::SetNextWindowSize(ImVec2(ibar_size, 45));

		//	ImGui::Begin(("info-bar"), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		//	{
		//		const ImVec2& pos = ImGui::GetWindowPos(), spacing = style->ItemSpacing, region = ImGui::GetContentRegionMax();

		//		ImGui::GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(ibar_size, 45), ImGui::GetColorU32(c::bg::background), c::child::rounding);
		//		ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, 10), pos + ImVec2(4, 35), ImGui::GetColorU32(c::accent), c::bg::rounding, ImDrawCornerFlags_Right);
		//		ImGui::GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(region.x - 4, 10), pos + ImVec2(region.x, 35), ImGui::GetColorU32(c::accent), c::bg::rounding, ImDrawCornerFlags_Left);
		//		ImGui::GetBackgroundDrawList()->AddRect(pos, pos + ImVec2(ibar_size, 45), ImGui::GetColorU32(c::child::outline), c::child::rounding);

		//		const char* info_set[4] = { cheat_name, developer, ping, world_time };
		//		static int info_bar_size = 0;

		//		ImGui::SetCursorPos(ImVec2(spacing.x, (45 - ImGui::CalcTextSize(developer).y) / 2));
		//		ImGui::BeginGroup();
		//		{

		//			for (int i = 0; i < 4; i++) {
		//				ImGui::TextColored(i < 1 ? ImColor(ImGui::GetColorU32(c::accent)) : ImColor(ImGui::GetColorU32(c::text::text)), info_set[i]);
		//				ImGui::SameLine();

		//				if (i < 3) {
		//					ImGui::TextColored(ImColor(ImGui::GetColorU32(c::text::text)), ("|"));
		//					ImGui::SameLine();
		//				}
		//			}
		//		}
		//		ImGui::EndGroup();
		//	}
		//	ImGui::End();
		//}

		ImGui::RenderNotifications();

	}
}

bool donewierdstuff = false;

void MainLoop()
{

	static RECT old_rc;
	if (!donewierdstuff)
	{
		ZeroMemory(&Message, sizeof(MSG));
		donewierdstuff = true;
	}

	if (PeekMessage(&Message, MyHwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.0f / 60.0f;

	GetCursorPos(&p);
	io.MousePos.x = p.x;
	io.MousePos.y = p.y;

	if (GetAsyncKeyState(0x1)) {
		io.MouseDown[0] = true;
		io.MouseClicked[0] = true;
		io.MouseClickedPos[0].x = io.MousePos.x;
		io.MouseClickedPos[0].x = io.MousePos.y;
	}
	else
		io.MouseDown[0] = false;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (SETTINGS::hdmiFuser) ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(Width, Height), IM_COL32(0, 0, 0, 255));

	render();
	Menu();

	ImGui::Render();
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (SETTINGS::vsync) {
		g_pSwapChain->Present(1, 0);
	}
	else {
		g_pSwapChain->Present(0, 0);
	}

	if (SETTINGS::hdmiFuser != SETTINGS::oldHdmiFuser) {
		if (SETTINGS::hdmiFuser) {
			SetWindowLong(MyHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);
			SetWindowPos(MyHwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else {
			SetWindowLong(MyHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
			SetWindowPos(MyHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		ShowWindow(MyHwnd, SW_SHOW);
		SetForegroundWindow(MyHwnd);
		UpdateWindow(MyHwnd);
		SETTINGS::oldHdmiFuser = SETTINGS::hdmiFuser;
	}
} 

//FORCEINLINE void fillOffsets()
//{
//	offsets::GameInstance = std::stoi;
//	offsets::GameState = std::stoi;
//}