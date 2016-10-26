#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2List.h"
#include "p2Point.h"
#include "Box2D/Box2D/Box2D.h"

struct SDL_Texture;


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();
	void RestartGame();

public:

	SDL_Texture* Lever_R;
	SDL_Texture* Lever_L;
	uint score;
	uint lives;
	uint lastscore;
	uint multiplier;
	uint bonus;
	/*Object RightFlipper;
	Object LeftFlipper;
	PhysBody* LeftFlipper_wheel;
	PhysBody* RightFlipper_wheel;*/
	char title[50];


};
