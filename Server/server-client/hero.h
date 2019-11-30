#pragma once
#include "character.h"
#include <queue>
#include <string>

using namespace std::chrono;

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

		void Attack(Character& enemy);
		void TakeDmg();
		void TakeHeal();
		virtual void NormalStrike(Character& enemy) {};
		virtual void SpecialStrike(Character& enemy) {};
		virtual int getRange() { return this->range; };
		void takeHeal();
		void takeDmg();
		virtual int getSpecDmg() { return 1; };
		virtual int getNormDmg() {return 1; };
		
		time_point<system_clock, seconds> getTimeOfStrike() { return this->TimeOfStrike; }
		time_point<system_clock, seconds> getTimeOfNormStrike() { return this->TimeOfNormStrike; }
		time_point<system_clock, seconds> getTimeOfSpecStrike() { return this->TimeOfSpecStrike; }

		seconds getCooldown() { return this->Cooldown; }
		seconds getNormCooldown() { return this->NormCooldown; }
		seconds getSpecCooldown() { return this->SpecCooldown; }

		void setCooldown(std::chrono::seconds cd);
		void setNormCooldown(std::chrono::seconds cd);
		void setSpecCooldown(std::chrono::seconds cd);

		void setTimeOfStrike(time_point<system_clock, seconds> time);
		void setTimeOfNormStrike(time_point<system_clock, seconds> time);
		void setTimeOfSpecStrike(time_point<system_clock, seconds> time);

		

	private:
		int range = 5;

		int maximum_MP;
		int current_MP;

		std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> TimeOfStrike;
		std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> TimeOfNormStrike;
		std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> TimeOfSpecStrike;
		seconds Cooldown = std::chrono::seconds(5);
		seconds NormCooldown = std::chrono::seconds(10);
		seconds SpecCooldown = std::chrono::seconds(20);

	};

	class Warrior : public Hero {
	public:
		Warrior(std::string name = "", int level = 1, int max_HP = 720, int curr_HP = 720, int attackRange = 10);
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

		int normal_dmg = 85;
		int special_dmg = 135;
		int dmg_taken = 0;
		int heal_taken = 0;
		int dmg_dealt = 0;
	};
};