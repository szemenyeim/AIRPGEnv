#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

//constexpr auto SPACE = 32;

class Environment
{
public:
	unsigned int SIZE_X;
	unsigned int SIZE_Y;

	unsigned int x_pos_onScreen;
	unsigned int y_pos_onScreen;

	void ClearWindow();
	void ShowWindow(int x, int y);

	Environment(const char* &img4map);
	void DrawHero(int x, int y);
	void DrawMonster(int x, int y);
	static int GetKeyPressed();

	int isBlue(int x, int y);

private:

	const char* &img4map;
	cv::Mat current_game;
};