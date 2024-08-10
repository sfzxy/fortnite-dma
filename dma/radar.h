#pragma once

static Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = (EntityPos.x - LocalPlayerPos.x);
	float Yaw = angle - 90;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= -zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return Vector3(x_1, y_1, 0);
}

void DrawRadarHUD(int xAxis, int yAxis, int width, int height)
{
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	ImVec2 pos(xAxis, yAxis);
	ImVec2 siz(width, height);
	ImVec2 RadarCenter(pos.x + siz.x / 2, pos.y + siz.y / 2);

	// Draw outline box
	ImU32 outlineColor = IM_COL32(31, 31, 31, 255); // Adjust color as needed
	drawList->AddRect(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), outlineColor);

	// Draw filled box
	ImU32 fillColor = IM_COL32(31, 31, 31, 230); // Adjust color as needed
	drawList->AddRectFilled(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), fillColor);

	// Draw lines
	ImU32 lineColor = IM_COL32(76, 76, 76, 255); // Adjust color as needed
	drawList->AddLine(RadarCenter, pos, lineColor);
	drawList->AddLine(RadarCenter, ImVec2(pos.x + siz.x, pos.y), lineColor);
	drawList->AddLine(ImVec2(pos.x, RadarCenter.y), ImVec2(pos.x + siz.x, RadarCenter.y), lineColor);
	drawList->AddLine(RadarCenter, ImVec2(RadarCenter.x, pos.y + siz.y), lineColor);

	// Draw circle
	float circleRadius = 2.0f;
	drawList->AddCircle(RadarCenter, circleRadius, lineColor, 12, 1.0f);
}

void DrawRadarPoint(Vector3 position, int distance, Vector3 localPosition, int scale, float zoom, bool distanceRadar, double localYaw, int xAxis, int yAxis, int width, int height, ImColor color)
{
	bool out = false;
	Vector3 siz;
	siz.x = width;
	siz.y = height;
	Vector3 pos;
	pos.x = xAxis;
	pos.y = yAxis;
	bool ck = false;
	Vector3 single = RotatePoint(position, localPosition, pos.x, pos.y, siz.x, siz.y, localYaw, zoom, &ck);
	if (distance >= 0.f && distance < scale)
	{
		if (distanceRadar) {
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(single.x, single.y), color, std::to_string((int)distance).c_str());
		}
		else {
			ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(single.x, single.y), 5, color);
		}
	
	}
}