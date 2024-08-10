#pragma once

std::unordered_map<int, std::string> nameCache;

static std::string GetNameFromIndex(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = TargetProcess->Read<uint64_t>(point::Base + offsets::FNamePool + ChunkOffset * 8 + 0x10) + 2 * NameOffset;
	uint16_t nameEntry = TargetProcess->Read<uint16_t>(NamePoolChunk);

	int nameLength = nameEntry >> 6;
	char buff[1024] = {};

	//void cache_c::decrypt_ascii(char* buff, int nameLength, unsigned short flags)
	//{
	//	char* v1 = buff; // rdi
	//	int v2; // ebx
	//	int v3; // eax
	//	int v4; // edx
	//	int v5; // ecx
	//	int v6; // ecx
	//	char v7; // al
	//	__int64 v8; // rax

	//	v2 = nameLength;

	//	v4 = 0;
	//	v5 = 30;
	//	if (v2)
	//	{
	//		do
	//		{
	//			v6 = v4++ | v5;
	//			v7 = v6;
	//			v5 = 2 * v6;
	//			v3 = ~v7;
	//			*v1++ ^= v3;
	//		} while (v4 < v2);
	//	}

	//	buff[nameLength] = '\0';
	//}

	char* v1 = buff; // rdi
	int v2; // ebx
	int v3; // eax
	int v4; // edx
	int v5; // ecx
	int v6; // ecx
	char v7; // al
	__int64 v8; // rax

	v2 = nameLength;

	v4 = 0;
	v5 = 30;
	if (v2 > 0 && v2 < 50)
	{
		TargetProcess->ReadString(NamePoolChunk + 2, buff, nameLength);
		do
		{
			v6 = v4++ | v5;
			v7 = v6;
			v5 = 2 * v6;
			v3 = ~v7;
			*v1++ ^= v3;
		} while (v4 < v2);
		buff[nameLength] = '\0';
		return std::string(buff);
	}

	return std::string(("N/A"));
}

static std::string GetNameFromFName(int key)
{
	if (nameCache.find(key) != nameCache.end()) {
		return nameCache[key];
	}

	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = TargetProcess->Read<uint64_t>(point::Base + offsets::FNamePool + ChunkOffset * 8 + 0x10) + 2 * NameOffset;
	uint16_t nameEntry = TargetProcess->Read<uint16_t>(NamePoolChunk);
	if (nameEntry < 64)
	{
		auto a1 = TargetProcess->Read<DWORD>(NamePoolChunk + 2);
		string nameF = GetNameFromIndex(a1);
		nameCache[key] = nameF;
		return nameF;
	}
	else
	{
		string nameF = GetNameFromIndex(key);
		nameCache[key] = nameF;
		return nameF;
	}
}

bool pickaxe = false;

enum weapons : int {
	RIFLE = 1,
	SHOTGUN = 2,
	SMG = 3,
	SNIPER = 4
};

struct LocalPlayer {

	weapons localWeapon;
	std::string localWeaponName;

} g_localPlayer;

struct Item {
	bool cached = true;
	int useCount = 0;
	bool isLoot = false;
	uint32_t FNameKey = 0;
	uintptr_t actor = 0;
	std::string name{};
	uintptr_t itemDefinition = 0;
	uintptr_t textPtr = 0;
	uintptr_t ftext_data = 0;
	int ftext_length = 0;
	wchar_t ftext_buf[50];
	std::string itemName{};
	uintptr_t rootComponent = 0;
	Vector3 Location{};
};

std::unordered_map<uintptr_t, Item> newItemCache;

std::unordered_map<uintptr_t, Item> newItemCache2;

std::unordered_map<uintptr_t, Item> readyItemCache;

struct PlayerCache {
	bool cached = true;

	uintptr_t PlayerState;
	uintptr_t Player;
	uintptr_t Mesh;
	uintptr_t BoneArray1;
	uintptr_t BoneArray2;
	uintptr_t RootComponent ;
	Vector3 Velocity;
	uint32_t TeamId ;
	int useCount;

	BYTE isDying = false; // & 0x10
	BYTE isDBNO = false;
	BYTE isBot = false;

	FTransform HeadBone{}; // 68
	FTransform BottomBone{}; // 0

	FTransform NeckBone{}; // 66
	FTransform HipBone{}; // 2
	FTransform UpperArmLeftBone{}; // 9
	FTransform UpperArmRightBone{}; // 38
	FTransform LeftHandBone{}; // 10
	FTransform RightHandBone{}; // 39
	FTransform LeftHand1Bone{}; // 11
	FTransform RightHand1Bone{}; // 40
	FTransform RightThighBone{}; // 78
	FTransform LeftThighBone{}; // 71
	FTransform RightCalfBone{}; // 79
	FTransform LeftCalfBone{}; // 72
	FTransform LeftFootBone{}; // 75
	FTransform RightFootBone{}; // 82

	FTransform component_to_world{};

	float last_submit = 0;
	float last_render = 0;

	bool gettingName = false;
	bool gotName = false;
	uintptr_t FString = 0;
	__int32 FStringLength = 0;
	uintptr_t FStringFText = 0;
	wchar_t NameBuffer[100];

	std::string name{};
};

PlayerCache PlayerArray[100];

std::unordered_map<uintptr_t, PlayerCache> newCache;

std::unordered_map<uintptr_t, PlayerCache> readyCache;

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
}fnRot;

Camera getCamera()
{

	Camera vCamera;

	if (!point::RotationPointer || !point::LocationPointer || !point::PlayerController) {
		vCamera.Location.x = 0;
		vCamera.Location.y = 0;
		vCamera.Location.z = 0;
		vCamera.Rotation.x = 0;
		vCamera.Rotation.y = 0;
		vCamera.Rotation.z = 0;
		vCamera.FieldOfView = 0;
		return vCamera;
	}

	float tempFOV;

	VMMDLL_SCATTER_HANDLE scatterReadHandle1 = TargetProcess->CreateScatterHandle(0);

	TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::RotationPointer, &fnRot, sizeof(FNRot));
	TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::LocationPointer, &vCamera.Location, sizeof(Vector3));
	TargetProcess->QueueScatterReadEx(scatterReadHandle1, point::PlayerController + 0x394, &tempFOV, sizeof(float));

	TargetProcess->ExecuteScatterRead(scatterReadHandle1);

	//fnRot = TargetProcess->Read<FNRot>(point::RotationPointer);

	//vCamera.Location = TargetProcess->Read<Vector3>(point::LocationPointer);

	vCamera.Rotation.x = asin(fnRot.c) * (180.0 / M_PI);
	vCamera.Rotation.y = atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI);

	vCamera.FieldOfView = tempFOV * 90.f; // TargetProcess->Read<float>(point::PlayerController + 0x394)

	return vCamera;

}

Camera mainCamera;

void updateCamera()
{
	mainCamera = getCamera();
}

Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	FORTNITEMATRIX tempMatrix = Matrix(mainCamera.Rotation, Vector3(0, 0, 0));

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - mainCamera.Location;

	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f) vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(mainCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(mainCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

Vector3 GetBoneWithRotation(uintptr_t bone_array, uintptr_t mesh, int id)
{
	SIZE_T size;

	FTransform bone = TargetProcess->Read<FTransform>((bone_array + (id * 0x60)));

	FTransform component_to_world = TargetProcess->Read<FTransform>((mesh + offsets::ComponetToWorld));

	FORTNITEMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());

	return Vector3(matrix._41, matrix._42, matrix._43);
}

Vector3 doMatrix(FTransform bone, FTransform component_to_world)
{
	FORTNITEMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());

	return Vector3(matrix._41, matrix._42, matrix._43);
}

std::unique_ptr<uintptr_t[]> readArray(uintptr_t address, int count)
{
	std::unique_ptr<uintptr_t[]> value(new uintptr_t[count]);

	TargetProcess->ReadString(address, reinterpret_cast<void*>(value.get()), sizeof(uintptr_t) * count);

	return value;
}

bool ShouldUsePrediction = false;
float bulletSpeed = 0;
float gravity = 0;

Vector3 predict_location(Vector3 target, Vector3 velocity, float distance)
{
	if (gravity > 0) {
		float time = distance / bulletSpeed;

		target.addScaled(velocity, time);

		float gravity2 = std::fabs(-980.0f * gravity) * 0.5f * time * time;
		target.z += gravity2;
	}

	return target;
}

Vector3 CalculatePrediction(Vector3 AimedBone, Vector3 Velocity, float Distance)
{
	if (gravity > 0) {
		float time = Distance / bulletSpeed;

		AimedBone.x += Velocity.x * time;
		AimedBone.y += Velocity.y * time;
		AimedBone.z += (Velocity.z * time) + abs(-980.f * gravity) * .5f * (time * time);
		return AimedBone;
	}
	
	return AimedBone;
}