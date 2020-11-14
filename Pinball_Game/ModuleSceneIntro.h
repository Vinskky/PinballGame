#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;
enum Area
{
	START = 1,
	FLOOR,
	SLIDE
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	void UpdateLevel(Area lvlToChange);

	//flippers
	SDL_Rect flipperRight;
	SDL_Rect flipperLeft;
	SDL_Rect flipperRightUp;
	SDL_Rect playerBall;

	//background
	SDL_Rect bgRect;

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> ballList;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> polligons;

	PhysBody* dieSensor;
	PhysBody* entryLevel;
	PhysBody* entrySlide;
	PhysBody* exitSlide;
	PhysBody* boostSlide;
	PhysBody* boostStairs;
	PhysBody* bumperLeft;
	PhysBody* bumperRight;
	PhysBody* boostFireHydrant;
	PhysBody* boostHouse;
	PhysBody* boostMask;
	PhysBody* boostTermometer;
	PhysBody* leftLifeSavour;
	PhysBody* rightLifeSavour;
	
	
	
	bool sensed;

	SDL_Texture* box;
	SDL_Texture* ball;
	SDL_Texture* rick;
	SDL_Texture* background;
	SDL_Texture* pinballSet;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;

	Area currentLvl;


	//Poligon Shapes
	int outsideBounds[112] = {
	201, 795,
	200, 771,
	78, 703,
	74, 735,
	43, 735,
	46, 505,
	73, 474,
	76, 460,
	12, 310,
	9, 263,
	10, 220,
	16, 183,
	32, 126,
	48, 96,
	95, 33,
	126, 12,
	166, 5,
	207, 10,
	242, 37,
	260, 57,
	298, 53,
	330, 55,
	396, 91,
	423, 122,
	472, 185,
	498, 235,
	510, 310,
	517, 494,
	518, 650,
	517, 756,
	516, 800,
	480, 796,
	475, 759,
	478, 668,
	473, 404,
	466, 295,
	440, 220,
	408, 171,
	357, 118,
	326, 101,
	309, 95,
	306, 140,
	384, 169,
	427, 203,
	449, 254,
	454, 288,
	454, 302,
	411, 348,
	414, 469,
	448, 484,
	460, 509,
	460, 737,
	429, 737,
	425, 705,
	300, 771,
	298, 797
	};

	int innerBound[90] = {
	176, 359,
	200, 340,
	202, 333,
	132, 256,
	117, 217,
	115, 172,
	191, 144,
	197, 136,
	196, 97,
	178, 56,
	166, 54,
	103, 90,
	60, 156,
	45, 197,
	39, 237,
	43, 291,
	83, 378,
	111, 425,
	116, 433,
	122, 430,
	93, 377,
	73, 344,
	51, 290,
	46, 227,
	51, 201,
	63, 165,
	74, 142,
	94, 114,
	111, 95,
	138, 116,
	118, 148,
	99, 182,
	89, 229,
	93, 280,
	101, 305,
	118, 345,
	144, 389,
	134, 367,
	114, 329,
	104, 300,
	97, 273,
	113, 263,
	121, 261,
	135, 295,
	170, 350
	};

	//tobogán
	int slide[92] = {
	146, 387,
	104, 316,
	9, 145,
	4, 130,
	6, 78,
	37, 28,
	74, 6,
	112, 0,
	160, 9,
	234, 42,
	294, 43,
	349, 37,
	404, 47,
	446, 67,
	494, 113,
	516, 154,
	524, 194,
	524, 240,
	511, 284,
	422, 477,
	416, 496,
	419, 579,
	386, 578,
	384, 520,
	400, 452,
	441, 366,
	486, 268,
	495, 214,
	492, 170,
	460, 120,
	425, 88,
	380, 69,
	354, 64,
	315, 67,
	283, 70,
	247, 70,
	211, 62,
	147, 36,
	116, 26,
	76, 32,
	46, 60,
	34, 105,
	45, 140,
	68, 183,
	104, 245,
	175, 366
	};

	int bouncerLeft[18] = {
	122, 548,
	130, 551,
	136, 569,
	150, 606,
	163, 634,
	163, 645,
	137, 636,
	120, 623,
	120, 552
	};

	int bouncerRight[16] = {
	378, 548,
	368, 558,
	338, 633,
	337, 645,
	345, 645,
	380, 625,
	381, 602,
	380, 551
	};

	int LLeft[26] = {
	75, 563,
	76, 648,
	87, 667,
	127, 693,
	150, 708,
	167, 709,
	171, 696,
	156, 687,
	97, 650,
	84, 625,
	85, 550,
	75, 549,
	75, 557
	};

	int LRight[28] = {
	417, 549,
	425, 549,
	425, 641,
	422, 650,
	417, 662,
	408, 672,
	341, 713,
	333, 710,
	330, 702,
	331, 693,
	379, 665,
	410, 644,
	417, 632,
	417, 558
	};

	int closeEntrance[14] = {
	319, 96,
	294, 69,
	270, 53,
	295, 40,
	340, 52,
	354, 79,
	336, 88
	};
};
