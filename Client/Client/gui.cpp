#include "Gui.h"

using namespace std;
using namespace cv;

const int hero_radius = 1;
const int RGB_BLUE = 250;

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

void GUI::ShowWindow(int x, int y)
{

	x = x - 32 > 0 ? x - 32 : 0;
	y = y - 32 > 0 ? y - 32 : 0;
	x = x + 64 < current_game.cols ? x : current_game.cols - 64;
	y = y + 64 < current_game.rows ? y : current_game.rows - 64;
	current_game = current_game(Rect(x, y, 64, 64));
	imshow(window_name, current_game);


	waitKey(1);
}

GUI::GUI(const char* &img4map, const char* &window_name) :img4map(img4map), window_name(window_name)
{
	ClearWindow();
	namedWindow(window_name, WINDOW_NORMAL);
	SIZE_X = current_game.cols;
	SIZE_Y = current_game.rows;

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
		Scalar(255, 0, 0),
		FILLED
	);
}

int GUI::GetKeyPressed()
{
	return waitKeyEx(1);
}

int GUI::isBlue(int x, int y)
{
	/*Blue means water in the map. The hero can not swimm so he cannot go to deep water */

	int blue = 0;
	ClearWindow();

	Vec3b colour = current_game.at<Vec3b>(Point(x, y));
	if (colour.val[0] > RGB_BLUE)
		blue = 1;
	return blue;
}