#include "hero.h"

using namespace RPGEnv;
using namespace std::chrono;

Hero::Hero(std::string name, int level, int max_HP, int curr_HP) :
	Character(level, max_HP, curr_HP)
{
	this->name = name;

	this->TimeOfStrike = time_point_cast<seconds>(system_clock::now());
	this->TimeOfNormStrike = time_point_cast<seconds>(system_clock::now());
	this->TimeOfSpecStrike = time_point_cast<seconds>(system_clock::now());
};

void Hero::setExplorationMatrix(int rows, int cols) {
	this->matrix_rows = rows;
	this->matrix_cols = cols;
	std::cout << "Rows, Cols: " << rows << "," << cols << std::endl;
	ExplorationMatrix = new int*[rows];
	for (int i = 0; i < rows; ++i) {
		ExplorationMatrix[i] = new int[cols];
		ExplorationMatrix[i] = { 0 };
	}

	for (int row = position.y - 32; row < position.y + 32; row++) {
		for (int col = position.x - 32; col < position.x + 32; col++) {
			ExplorationMatrix[row][col] = 1;
		}
	}
};

void Hero::Die() {
	delete ExplorationMatrix;
	delete this;
}


void Hero::Exploring(int keypressed) {
	switch (keypressed)
	{
	case (int)'w':
	{
		for (int col = position.x - 32; col < position.x + 32; col++) //north
		{
			if (position.x + 32 <= this->matrix_cols) {
				if (ExplorationMatrix[position.y + 32][col] == 0)
				{
					ExplorationMatrix[position.y + 32][col] = 1;
					experience += 0.1;
				}
			}
		}
		std::cout << *ExplorationMatrix[position.y + 32] << std::endl;
		break;
	}
	case (int)'s':
	{
		for (int col = position.x - 32; col < position.x + 32; col++) //south
		{
			if (position.y - 32 >= 0) {
				if (ExplorationMatrix[position.y - 32][col] == 0)
				{
					ExplorationMatrix[position.y - 32][col] = 1;
					experience += 0.1;
				}
			}
		}
		break;
	}

	case (int)'a':
	{
		for (int row = position.y - 32; row < position.y + 32; row++) //west
		{
			if (position.x - 32 >= 0) {
				if (ExplorationMatrix[row][position.x - 32] == 0)
				{
					ExplorationMatrix[row][position.x - 32] = 1;
					experience += 0.1;
				}
			}
		}
		break;
	}
	case (int)'d':
	{
		for (int row = position.y - 32; row < position.y + 32; row++) //east
		{
			if (position.x + 32 <= this->matrix_rows) {
				if (ExplorationMatrix[row][position.x + 32] == 0)
				{
					ExplorationMatrix[row][position.x + 32] = 1;
					experience += 0.1;
				}
			}
		}

		break;
	}
	}

}

std::string Hero::Parse()
{
	std::string msg;
	msg = std::to_string(id) + ";" + name + ";" + std::to_string(position.x) + ";"
		+ std::to_string(position.y) + ";"
		+ std::to_string(Level) + ";"
		+ std::to_string(current_HP) + ";"
		+ std::to_string(experience) + "\n";
	return msg;
}

void RPGEnv::Hero::Attack(Character& enemy)
{
	if ((time_point_cast<seconds>(system_clock::now()) - this->getTimeOfStrike()) > this->getCooldown()) {
		enemy.current_HP -= 15;//Level * 5;
		this->experience += 15;
		std::cout << enemy.name << " HP: " << enemy.current_HP << std::endl;

		this->setTimeOfStrike(time_point_cast<seconds>(system_clock::now()));
	}
}

void RPGEnv::Hero::setCooldown(std::chrono::seconds cd)
{
	this->Cooldown = cd;
}

void RPGEnv::Hero::setNormCooldown(std::chrono::seconds cd)
{
	this->NormCooldown = cd;
}

void RPGEnv::Hero::setSpecCooldown(std::chrono::seconds cd)
{
	this->SpecCooldown = cd;
}

void RPGEnv::Hero::setTimeOfStrike(time_point<system_clock, seconds> time)
{
	this->TimeOfNormStrike = time;
}

void RPGEnv::Hero::setTimeOfNormStrike(time_point<system_clock, seconds> time)
{
	this->TimeOfNormStrike = time;
}

void RPGEnv::Hero::setTimeOfSpecStrike(time_point<system_clock, seconds> time)
{
	this->TimeOfSpecStrike = time;
}


// -------------------------------------------WARRIOR---------------------------------------------------------
Warrior::Warrior(std::string name, int level, int max_HP, int curr_HP, int attackRange) :
	Hero(name, level, max_HP, curr_HP)
{
	this->attackRange = attackRange;
	this->id = 98;

}

void Warrior::SpecialStrike(Character& enemy) {
	auto now = time_point_cast<seconds>(system_clock::now());
	auto timeofstrike = this->getTimeOfSpecStrike();
	auto cd = this->getSpecCooldown();
	if ((time_point_cast<std::chrono::seconds>(system_clock::now()) - this->getTimeOfSpecStrike()) > this->getSpecCooldown()) {
		enemy.current_HP -= this->special_dmg;//Level * 5;
		this->experience += this->special_dmg;
		std::cout << enemy.name << " HP: " << enemy.current_HP << std::endl;
		this->setTimeOfSpecStrike(time_point_cast<seconds>(system_clock::now()));
	}
}

void Warrior::NormalStrike(Character& enemy) {

	if ((time_point_cast<std::chrono::seconds>(system_clock::now()) - this->getTimeOfNormStrike()) > this->getNormCooldown()) {
		enemy.current_HP -= this->normal_dmg;//Level * 5;
		this->experience += this->normal_dmg;
		std::cout << enemy.name << " HP: " << enemy.current_HP << std::endl;
		this->setTimeOfNormStrike(time_point_cast<seconds>(system_clock::now()));
	}
}

void Warrior::takeDmg(int dmg) {
	this->current_HP -= dmg;
	this->dmg_taken += dmg;
}

int Warrior::getRange() {
	return this->attackRange;
}
// ------------------------------------------MAGE---------------------------------------------------------
Mage::Mage(std::string name, int level, int max_HP, int curr_HP, int attackRange) :
	Hero(name, level, max_HP, curr_HP)
{
	this->attackRange = attackRange;
	this->id = 99;

}

void Mage::SpecialStrike(Character& enemy) {
	auto now = std::chrono::time_point_cast<seconds>(system_clock::now());
	auto timeofstrike = this->getTimeOfSpecStrike();
	auto cd = this->getSpecCooldown();
	if ((time_point_cast<std::chrono::seconds>(system_clock::now()) - this->getTimeOfSpecStrike()) > this->getSpecCooldown()) {
		enemy.current_HP -= this->special_dmg;
		this->experience += this->special_dmg;
		std::cout << enemy.name << " HP: " << enemy.current_HP << std::endl;
		this->setTimeOfSpecStrike(time_point_cast<seconds>(system_clock::now()));
	}
}

void Mage::NormalStrike(Character& enemy) {

	if ((time_point_cast<std::chrono::seconds>(system_clock::now()) - this->getTimeOfNormStrike()) > this->getNormCooldown()) {
		enemy.current_HP -= this->normal_dmg;//Level * 5;
		this->experience += this->normal_dmg;
		std::cout << enemy.name << " HP: " << enemy.current_HP << std::endl;
		this->setTimeOfNormStrike(time_point_cast<seconds>(system_clock::now()));
	}
}

int Mage::getRange() {
	return this->attackRange;
}

