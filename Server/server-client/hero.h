#pragma once
#include "character.h"
#include <queue>
#include <string>



namespace RPGEnv {

	class Hero : public Character {
	public:
		std::string name;
		bool Attacking;
		bool Defensing;
		int** ExplorationMatrix;
		int matrix_rows;
		int matrix_cols;
		Hero(std::string name = "", int level = 1, int max_HP = 100, int curr_HP = 100);

		void setExplorationMatrix(int rows, int cols);

		void Die();
		void Exploring(int keypressed);
		std::string Parse();
		void TakeDmg();
		void TakeHeal();
		virtual void NormalStrike(Character& enemy) {};
		virtual void SpecialStrike(Character& enemy) {};
		virtual int getRange() { return this->range; };
		void takeHeal();
		void takeDmg();
		virtual int getSpecDmg() {};
		virtual int getNormDmg() {};
	private:
		int range = 5;

		int maximum_MP;
		int current_MP;

	};

	class Warrior : public Hero {
	public:
		Warrior(std::string name = "", int level = 1, int max_HP = 720, int curr_HP = 720, int attackRange = 5);
		void SpecialStrike(Character& enemy);

		void DefensiveStance();
		void takeDmg(int dmg);
		void takeHeal(int heal);
		void NormalStrike(Character& enemy);
		int getRange();
		int getSpecDmg() { return this->special_dmg; };
		int getNormDmg() { return this->normal_dmg; };
	private:
		int attackRange;
		std::chrono::system_clock::time_point TimeOfMeleeStrike;
		std::chrono::milliseconds meleeCooldown;

		int normal_dmg = 60;
		int special_dmg = 120;
		int dmg_taken = 0;
		int heal_taken = 0;
		int dmg_dealt = 0;

	};
	class Mage : public Hero {
	public:
		Mage(std::string name = "", int level = 1, int max_HP = 600, int curr_HP = 600, int attackRange = 25);
		void SpecialStrike(Character& enemy);
		void takeHeal(int heal);
		void takeDmg(int dmg);
		void NormalStrike(Character& enemy);
		int getRange();
		int getSpecDmg() { return this->special_dmg; };
		int getNormDmg() { return this->normal_dmg; };
	private:
		int attackRange;
		std::chrono::system_clock::time_point TimeOfRangedStrike;
		std::chrono::milliseconds rangedCooldown;
		int normal_dmg = 85;
		int special_dmg = 135;
		int dmg_taken = 0;
		int heal_taken = 0;
		int dmg_dealt = 0;
	};
};