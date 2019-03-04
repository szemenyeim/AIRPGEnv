#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

constexpr auto SPACE = 32;

class GUI 
{
public:
	unsigned int SIZE_X;
	unsigned int SIZE_Y;

	
	void ClearWindow();
	void ShowWindow(int x, int y);

	//Need to send a cropped segment 64x64 to show
	GUI(const char* &img4map, const char* &window_name);
	void DrawHero(int x, int y);
	void DrawMonster(int x, int y);
	static int GetKeyPressed();

private:

	const char* &window_name;
	const char* &img4map;
	cv::Mat current_game;
};

