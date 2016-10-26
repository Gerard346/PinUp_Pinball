#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool CreateMap();

	
public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> map;
	p2List<PhysBody*> bouncers;
	PhysBody* sensor;
	bool sensed;



	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* map_texture;
	SDL_Texture* map_texture_walls;
	SDL_Texture* Lever_L;
	SDL_Texture* Lever_R;
	SDL_Texture* title;
	SDL_Texture* light;
	SDL_Texture* numbers_others;
	SDL_Texture* numbers_score;
	SDL_Texture* piston_texture;
	SDL_Rect light_bulb;

	p2List_item<PhysBody*>* flips_start;

	PhysBody* P_Lever_L;
	PhysBody* P_Lever_R;
	PhysBody* dead_sensor;

	PhysBody* PivotLever_L;
	PhysBody* PivotLever_R;

	PhysBody* BodyLever_L;
	PhysBody* BodyLever_R;

	PhysBody* CircleLever_L;
	PhysBody* CircleLever_R;

	PhysBody* Piston;
	PhysBody* Piston2;

	uint intro_fx;
	uint bonus_fx;
	uint throw_fx;
	uint lever_fx;
	uint triangles_fx;
	uint ding_fx;
	uint bulb_fx;
	uint dead_fx;

	p2Point<int> ray;

	bool fx_lever_left = false;
	bool fx_lever_right = false;
	bool fx_intro = false;
	bool ray_on;
	bool spawned = false;
	bool dead = false;
	bool light1 = false;
	bool light2 = false;
	bool light3 = false;
	bool light4 = false;
	bool l1 = false;
	bool l2 = false;
	bool l3 = false;
	bool l4 = false;
	bool lost = false;
};
