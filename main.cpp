#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"

using namespace std;

class Example : public olc::PixelGameEngine {
	float playerX = 1.5f;
	float playerY = 15.5f;
	float playerAngle = 0.0f;
	int mapHeight = 64;
	int mapWidth = 64;
	wstring map;
	const float fieldOfView = 3.14 / 4.0;
	const float halfOfFieldOfView = fieldOfView / 2.0f;
	const float depth = 24.0f;
	float deltaAngle;
public:

	Example() {
		map += L"################################################################";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#...##################################.........................#";
		map += L"#..............................................................#";
		map += L"#...........................####################...............#";
		map += L"#...........................####################...............#";
		map += L"#...........................####################...............#";
		map += L"#...........................####################...............#";
		map += L"#..............................................................#";
		map += L"#................................#####...#######################";
		map += L"#....#####################.......#####.........................#";
		map += L"#....#####################.......#####.........................#";
		map += L"#....#####################.......#########################.....#";
		map += L"#....#####################.......#########################.....#";
		map += L"#....#####################.......#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#................................#####.........................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"#..............................................................#";
		map += L"################################################################";
		sAppName = "RayCasting";
	}

	bool OnUserCreate() override {
		deltaAngle = fieldOfView / static_cast<float>(ScreenWidth());
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		int32_t width = ScreenWidth();
		int32_t height = ScreenHeight();
		float distanceToWall;

		if (GetKey(olc::Q).bHeld) playerAngle -= fElapsedTime * 3.14f / 2.0f;

		if (GetKey(olc::E).bHeld) playerAngle += fElapsedTime * 3.14f / 2.0f;

		if (GetKey(olc::S).bHeld) {
			float newPlayerX = playerX - fElapsedTime * sinf(playerAngle) * 5.0f;
			float newPlayerY = playerY - fElapsedTime * cosf(playerAngle) * 5.0f;
			setNewPlayerPosition(newPlayerX, newPlayerY);
		}
		if (GetKey(olc::W).bHeld) {
			float newPlayerX = playerX + fElapsedTime * sinf(playerAngle) * 5.0f;
			float newPlayerY = playerY + fElapsedTime * cosf(playerAngle) * 5.0f;
			setNewPlayerPosition(newPlayerX, newPlayerY);
		}

		if (GetKey(olc::A).bHeld) {
			float newPlayerX = playerX - fElapsedTime * cosf(playerAngle) * 5.0f;
			float newPlayerY = playerY + fElapsedTime * sinf(playerAngle) * 5.0f;
			setNewPlayerPosition(newPlayerX, newPlayerY);
		}

		if (GetKey(olc::D).bHeld) {
			float newPlayerX = playerX + fElapsedTime * cosf(playerAngle) * 5.0f;
			float newPlayerY = playerY - fElapsedTime * sinf(playerAngle) * 5.0f;
			setNewPlayerPosition(newPlayerX, newPlayerY);
		}

		for (int x = 0; x < width; x++) {
			float rayAngle = playerAngle - halfOfFieldOfView + deltaAngle * static_cast<float>(x);
			float unitVectorX = sinf(rayAngle);
			float unitVectorY = cosf(rayAngle);
			bool hitWall = false;
			bool hitBoundary = false;
			distanceToWall  = 0.0f;
			int currentRayX;
			int currentRayY;

			while (!hitWall && distanceToWall < depth) {
				distanceToWall += 0.01f;
				float exactX = playerX + distanceToWall * unitVectorX;
				currentRayX = static_cast<int>(exactX);
				float exactY = playerY + distanceToWall * unitVectorY;
				currentRayY = static_cast<int>(exactY);
				if (currentRayX < 0 || currentRayY < 0 || currentRayX >= mapWidth || currentRayY >= mapHeight) {
					hitWall = true;
					distanceToWall = depth;
				} else if (map[mapWidth * currentRayY + currentRayX] == '#') {
					hitWall = true;
					int nearestX = static_cast<int>(std::round(exactX));
					int nearestY = static_cast<int>(std::round(exactY));
					float deltaX = abs(static_cast<float>(nearestX) - exactX);
					float deltaY = abs(static_cast<float>(nearestY) - exactY);
					if (sqrt(deltaX * deltaX + deltaY * deltaY) < 0.01) hitBoundary = true;
				}
			}

			int color;
			if (distanceToWall > depth) color = 0.0f;
			else color = static_cast<int>(192.0f * (1.0f - distanceToWall / depth) + 0.0f * (distanceToWall / depth));
			int ceiling = max(0, static_cast<int>(static_cast<float>(height) / 2.0f -
												  static_cast<float>(height) /
												  (distanceToWall * cosf(playerAngle - rayAngle))));
			int floor = height - ceiling;
			for (int32_t y = 0; y < height; y++) {
				if (y < ceiling) {
					Draw(x, y, olc::BLUE);
					continue;
				}
				if (y > floor) {
					float floorShadeCoefficient = 2.0f * (1 - static_cast<float>(y) / static_cast<float>(height));
					int greenShadeFloorColor = static_cast<int>(0.0f * floorShadeCoefficient +
																(1 - floorShadeCoefficient) * 150.0f);
					Draw(x, y, olc::Pixel(0, greenShadeFloorColor, 0));
					continue;
				}
				Draw(x, y, hitBoundary ? olc::BLACK : olc::Pixel(color, color, color, 255));
			}
		}

		for (int nx = 0; nx < mapWidth; nx++)
			for (int ny = 0; ny < mapHeight; ny++) {
				olc::Pixel color;
				color = map[ny * mapWidth + nx] == '#' ? olc::BLACK : olc::CYAN;
				Draw(nx, ny, color);
			}
		Draw((int) playerX, (int) playerY, olc::RED);
		return true;
	}

	void setNewPlayerPosition(float newPlayerX, float newPlayerY) {
		if (!isNewPositionInBoundaries(newPlayerX, newPlayerY)) return;
		playerX = newPlayerX;
		playerY = newPlayerY;
	}

	bool isNewPositionInBoundaries(float newPlayerX, float newPlayerY) {
		return newPlayerX - 0.11 >= 0 &&
			   newPlayerY - 0.11 >= 0 &&
			   newPlayerX + 0.11 < static_cast<float>(mapWidth) &&
			   newPlayerY + 0.11 < static_cast<float>(mapHeight) &&
			   map[static_cast<int>(newPlayerY) * mapWidth + static_cast<int>(newPlayerX)] != '#';
	}
};


int main() {
	Example demo;
	if (demo.Construct(1600, 720, 1, 1))
		demo.Start();

	return 0;
}