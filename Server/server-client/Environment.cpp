#include "Environment.h"

using namespace std;
using namespace cv;

const int hero_radius = 3;
const int RGB_BLUE = 250;

class Circle
{
public:

	int x;
	int y;
	int radius;

	Circle(int &x, int &y, int &radius) : x(x), y(y), radius(radius) {};
};

void Environment::ClearWindow()
{
	Mat game_map = imread(img4map);
	current_game = game_map.clone();
}

Environment::Environment(const char* &img4map) :img4map(img4map)
{
	ClearWindow();

	/*TODO: Teszt*/
	namedWindow("teszt", WINDOW_NORMAL);
	SIZE_X = current_game.cols;
	SIZE_Y = current_game.rows;

}

void Environment::ShowWindow()
{
	imshow("teszt", current_game);
	waitKey(1);
}


void Environment::DrawHero(int x, int y)
{
	circle(
		current_game,
		Point(x, y),
		hero_radius,
		Scalar(0, 0, 255),
		FILLED
	);
}

void Environment::DrawMonster(int x, int y)
{
	circle(
		current_game,
		Point(x, y),
		hero_radius,
		Scalar(255, 0, 0),
		FILLED
	);
}

int Environment::GetKeyPressed()
{
	return waitKeyEx();
}

int Environment::isBlue(int x, int y)
{
	/*Blue means water in the map. The hero can not swimm so he cannot go to deep water */

	int blue = 0;
	ClearWindow();

	Vec3b colour = current_game.at<Vec3b>(Point(x, y));
	if (colour.val[0] > RGB_BLUE)
		blue = 1;
	return blue;
}