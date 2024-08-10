#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include <atomic>
#include <mutex>
//#include <TlHelp32.h>
#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>
#include <xmmintrin.h>
#include <array>
#include <vector>
#include <cstdlib>
#include <random>
#include <direct.h>
#include <fstream>
#include <string>
#include <sstream>

#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")

#include "render/ImGui/imgui.h"
#include "render/ImGui/imgui_impl_dx11.h"
#include "render/ImGui/imgui_impl_win32.h"
#include "render/ImGui/imgui_internal.h"
#include "render/ImGui/imgui_freetype.h"

extern HWND GameHwnd;
extern RECT GameRect;
extern HWND MyHwnd;
extern MSG Message;

static int Width = GetSystemMetrics(SM_CXSCREEN);
static int Height = GetSystemMetrics(SM_CYSCREEN);

extern class Vector2;
extern class Vector3;

class Util {
public:
	static HWND get_process_wnd(uint32_t pid);
	static int get_fps();
	static int get_fps2();
	static void DrawCornerBox(int X, int Y, int W, int H, const ImColor color, int thickness);
	static double GetCrossDistance(double x1, double y1, double x2, double y2);
	static void DrawBox(float X, float Y, float W, float H, const ImU32& color, int thickness);
};

#define M_PI 3.14159265358979323846

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(double _x, double _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	double x;
	double y;
};
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	static double GetDistance(Vector3 point1, Vector3 point2)
	{
		Vector3 heading = point2 - point1;
		double distanceSquared;
		double distance;

		distanceSquared = heading.x * heading.x + heading.y * heading.y + heading.z * heading.z;
		distance = sqrt(distanceSquared);

		return distance;
	}

	void addScaled(const Vector3& v, float scale)
	{
		x += v.x * scale;
		y += v.y * scale;
		z += v.z * scale;
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(double number) const {
		return Vector3(x * number, y * number, z * number);
	}

	Vector3 operator/(double number) const {
		return Vector3(x / number, y / number, z / number);
	}
};
struct FQuat
{
	double x;
	double y;
	double z;
	double w;
};

typedef struct FORTNITEMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad01[4];
	Vector3 scale;
	FORTNITEMATRIX ToMatrixWithScale()
	{
		FORTNITEMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
class FRotator
{
public:
	FRotator() : Pitch(0.f), Yaw(0.f), Roll(0.f)
	{

	}

	FRotator(double _Pitch, double _Yaw, double _Roll) : Pitch(_Pitch), Yaw(_Yaw), Roll(_Roll)
	{

	}
	~FRotator()
	{

	}

	double Pitch;
	double Yaw;
	double Roll;
	inline FRotator get() {
		return FRotator(Pitch, Yaw, Roll);
	}
	inline void set(double _Pitch, double _Yaw, double _Roll) {
		Pitch = _Pitch;
		Yaw = _Yaw;
		Roll = _Roll;
	}

	inline FRotator Clamp() {
		FRotator result = get();
		if (result.Pitch > 180)
			result.Pitch -= 360;
		else if (result.Pitch < -180)
			result.Pitch += 360;
		if (result.Yaw > 180)
			result.Yaw -= 360;
		else if (result.Yaw < -180)
			result.Yaw += 360;
		if (result.Pitch < -89)
			result.Pitch = -89;
		if (result.Pitch > 89)
			result.Pitch = 89;
		while (result.Yaw < -180.0f)
			result.Yaw += 360.0f;
		while (result.Yaw > 180.0f)
			result.Yaw -= 360.0f;

		result.Roll = 0;
		return result;

	}
	double Length() {
		return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	FRotator operator+(FRotator angB) { return FRotator(Pitch + angB.Pitch, Yaw + angB.Yaw, Roll + angB.Roll); }
	FRotator operator-(FRotator angB) { return FRotator(Pitch - angB.Pitch, Yaw - angB.Yaw, Roll - angB.Roll); }
	FRotator operator/(float flNum) { return FRotator(Pitch / flNum, Yaw / flNum, Roll / flNum); }
	FRotator operator*(float flNum) { return FRotator(Pitch * flNum, Yaw * flNum, Roll * flNum); }
	bool operator==(FRotator angB) { return Pitch == angB.Pitch && Yaw == angB.Yaw && Yaw == angB.Yaw; }
	bool operator!=(FRotator angB) { return Pitch != angB.Pitch || Yaw != angB.Yaw || Yaw != angB.Yaw; }

};

extern FORTNITEMATRIX MatrixMultiplication(FORTNITEMATRIX pM1, FORTNITEMATRIX pM2);
extern FORTNITEMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0));

struct Camera
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
	char Useless[0x18];
};

struct TempCamera
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
};

struct FMinimalViewInfo
{
	Vector3 Location;
	Vector3 Rotation;
	float FOV;
	float DesiredFOV;
};

template<class type>
class tarray
{
public:
	tarray() : data(nullptr), count(std::int32_t()), maxx(std::int32_t()) { }
	tarray(type* data, std::int32_t count, std::int32_t maxx) : data(data), count(count), maxx(maxx) { }

	const bool is_valid() const noexcept
	{
		return !(this->data == nullptr);
	}

	const std::int32_t size() const noexcept
	{
		return this->count;
	}

	type& operator[](std::int32_t index) noexcept
	{
		return this->data[index];
	}

	const type& operator[](std::int32_t index) const noexcept
	{
		return this->data[index];
	}

	bool is_valid_index(std::int32_t index) const noexcept
	{
		return index < this->size();
	}

protected:
	type* data;
	std::int32_t count;
	std::int32_t maxx;
};