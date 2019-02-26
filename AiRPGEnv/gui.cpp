#include "Gui.h"

using namespace std;
using namespace cv;

const int hero_radius = 1;

class Circle 
{
	public:

		int x;
		int y;
		int radius;

		Circle(int &x, int &y, int &radius) : x(x), y(y), radius(radius) {};
};

void GUI::ClearWindow()
{
	Mat game_map = imread(img4map);
	current_game = game_map.clone();
}

void GUI::ShowWindow()
{
	imshow(window_name, current_game);
	waitKey(1);
}

GUI::GUI(const char* &img4map, const char* &window_name) :img4map(img4map), window_name(window_name)
{
	ClearWindow();

	namedWindow(window_name, WINDOW_NORMAL);
	
	ShowWindow();
}

void GUI::DrawHero(int x, int y)
{
	circle(
		current_game,
		Point(x, y),
		hero_radius,
		Scalar(0, 0, 255),
		FILLED
	);
}

void GUI::DrawMonster(int x, int y)
{
	circle(
		current_game,
		Point(x, y),
		hero_radius,
		Scalar(255, 0,0 ),
		FILLED
	);
}

int GUI::GetKeyPressed()
{
	return waitKeyEx(50);
}
