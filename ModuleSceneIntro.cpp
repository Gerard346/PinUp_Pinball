#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = map_texture = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	
	intro_fx = App->audio->LoadFx("pinball/Fx/intro.wav");
	bonus_fx = App->audio->LoadFx("pinball/Fx/bonus.wav");
	throw_fx = App->audio->LoadFx("pinball/Fx/throw.wav");
	lever_fx = App->audio->LoadFx("pinball/Fx/leveror.wav");
	triangles_fx = App->audio->LoadFx("pinball/Fx/triangles.wav");
	ding_fx = App->audio->LoadFx("pinball/Fx/ding.wav");
	bulb_fx = App->audio->LoadFx("pinball/Fx/bulb.wav");
	dead_fx = App->audio->LoadFx("pinball/Fx/dead.wav");

	circle = App->textures->Load("pinball/Sprites/Ball.png"); 
	box = App->textures->Load("pinball/Sprites/crate.png");
	map_texture = App->textures->Load("pinball/Sprites/Map_Pinball.png");
	Lever_L = App->textures->Load("pinball/Sprites/LeftFlipper.png");
	Lever_R = App->textures->Load("pinball/Sprites/RightFlipper.png");
	
	numbers_others = App->textures->Load("pinball/Sprites/Numbers_Others.png");
	numbers_score = App->textures->Load("pinball/Sprites/Numbers_Score.png");
	light = App->textures->Load("pinball/Sprites/Light_Bulb.png");
	title = App->textures->Load("pinball/Sprites/Title_Sprites.png");

	light_bulb = { 0,0,18,17 };

	//App->physics->CreatePrismaticJoint(0,0, 0, 0, 0, 0, 0, 0, 0);
	//App->physics->CreatePrismaticJoint(0, 0, 0, 0, 0, 0, 0, , 0);	

	CircleLever_L = App->physics->CreateCircle(167, 837, 7, 0, NONE);
	BodyLever_L = App->physics->CreateRectangle(192, 837, 45, 12, 1, Lever_L);
	PivotLever_L = App->physics->CreateCircle(192, 837, 6, 1,  NONE);

	App->physics->CreateRevolutionJoint(BodyLever_L, CircleLever_L, -0.5, 0, 0, 0, 0, 25, -20);
	App->physics->CreateRevolutionJoint(BodyLever_L, PivotLever_L, 0.35, 0, 0, 0, 0, 25, -20);

	CircleLever_R = App->physics->CreateCircle(286, 837, 7, 0, NONE);
	BodyLever_R = App->physics->CreateRectangle(261, 837, 45, 12, 1, Lever_R);
	PivotLever_R = App->physics->CreateCircle(261, 837, 6, 1, NONE);

	App->physics->CreateRevolutionJoint(BodyLever_R, CircleLever_R, 0.5, 0, 0, 0, 0, 21, -20);
	App->physics->CreateRevolutionJoint(BodyLever_R, PivotLever_R, -0.35, 0, 0, 0, 0, 21, -20);

	CreateMap();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = map.getFirst();


	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(map_texture, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(circle, x - 14.0f, y-3.0f, NULL, 1.0f);
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x - 5, y - 10, NULL, 1.0f, c->data->GetRotation());

		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	int x, y;
	BodyLever_L->GetPosition(x, y);
	App->renderer->Blit(Lever_L, x - 9, y - 12, NULL, 1.0f, BodyLever_L->GetRotation()-6);

	BodyLever_R->GetPosition(x, y);
	App->renderer->Blit(Lever_R, x - 1, y - 12, NULL, 1.0f, BodyLever_R->GetRotation()+6);

	PivotLever_L->body->ApplyForceToCenter(b2Vec2(0, 10), true);
	PivotLever_R->body->ApplyForceToCenter(b2Vec2(0, 10), true);
	

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		fx_lever_left = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (fx_lever_left == false)
		{
			App->audio->PlayFx(lever_fx, 0);
			fx_lever_left = true;
		}
		PivotLever_L->body->ApplyForceToCenter(b2Vec2(0, -90), true);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		fx_lever_right = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (fx_lever_right == false)
		{
			App->audio->PlayFx(lever_fx, 0);
			fx_lever_right = true;
		}
		PivotLever_R->body->ApplyForceToCenter(b2Vec2(0, -90), true);
	}
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 9, true, BALL));
		circles.getLast()->data->listener = this;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && spawned == false)
	{
		circles.add(App->physics->CreateCircle(465, 883, 9, true, BALL));
		spawned = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		circles.getFirst()->data->body->ApplyForceToCenter(b2Vec2(0.0f, -160.0f), true);
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}
/*if (App->player->score > 29999) {
		App->player->Lives++;
		)

		if (dead == true)
		{
		App->audio->PlayFx(dead_fx, 0);

		if (App->player->Lives > 1)
		{
		circles.add(App->physics->CreateCircle(455, 853, 9, true));
		App->player->multiplier=1;
		App->player->Lives--;
		}

		else
		{
		//App->player->Restart_game();

		}
		dead = false;
		}*/

		/*
		if (light1 == true){
		App->audio->PlayFx(bulb_fx, 0);
		App->render->Blit(light, 207, 265, &ready2);
		}
		else if (light2 == true){
		App->audio->PlayFx(bulb_fx, 0);
		App->renderer->Blit(light, 235, 265, &light_bulb);
		}
		else if (light3 == true){
		App->audio->PlayFx(bulb_fx, 0);
		App->renderer->Blit(light, 263, 265, &light_bulb);
		}
		else if (light4 == true){
		App->audio->PlayFx(bulb_fx, 0);
		App->renderer->Blit(light, 291, 265, &light_bulb);
		}

		if (light1 == true & light2 == true & light3 == true & light4 == true )
		{
		light1 == false;
		light2 == false;
		light3 == false;
		light4 == false;
		App->player->multiplier++;
		}
		*/

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->physics->sensor_collision(bodyA, bodyB);
}


bool ModuleSceneIntro::CreateMap()
{
	int Map_Pinball[120] = {
		454, 240,
		456, 318,
		449, 349,
		448, 413,
		464, 426,
		474, 443,
		477, 468,
		477, 894,
		470, 898,
		460, 898,
		455, 894,
		454, 476,
		449, 477,
		449, 493,
		413, 523,
		413, 559,
		448, 592,
		449, 786,
		444, 797,
		438, 804,
		266, 886,
		266, 924,
		186, 924,
		186, 886,
		20, 807,
		10, 797,
		5, 786,
		4, 583,
		37, 562,
		37, 521,
		84, 485,
		63, 455,
		32, 472,
		32, 485,
		25, 490,
		14, 490,
		6, 484,
		4, 172,
		7, 162,
		14, 153,
		22, 147,
		34, 145,
		146, 145,
		145, 220,
		148, 227,
		149, 241,
		146, 258,
		143, 267,
		133, 281,
		128, 281,
		124, 283,
		123, 288,
		125, 291,
		110, 311,
		105, 312,
		103, 317,
		106, 321,
		111, 321,
		147, 271,
		154, 253
	};

	int down2left[32] = {
		32, 608,
		32, 619,
		32, 773,
		33, 779,
		42, 785,
		149, 839,
		154, 839,
		157, 832,
		161, 828,
		166, 827,
		161, 821,
		48, 730,
		42, 723,
		38, 717,
		38, 607,
		34, 605
	};

	int downleft[24] = {
		71, 638,
		67, 641,
		64, 647,
		64, 707,
		66, 711,
		124, 760,
		131, 762,
		139, 759,
		141, 752,
		83, 642,
		78, 638,
		73, 638
	};

	int right2left[32] = {
		414, 610,
		416, 606,
		419, 606,
		421, 609,
		421, 773,
		419, 778,
		415, 783,
		300, 840,
		297, 841,
		296, 836,
		293, 831,
		288, 826,
		404, 732,
		412, 722,
		415, 716,
		414, 612
	};

	int rightleft[26] = {
		389, 645,
		389, 707,
		387, 711,
		382, 715,
		328, 761,
		322, 762,
		317, 761,
		313, 757,
		313, 749,
		371, 640,
		376, 638,
		383, 639,
		388, 643
	};

	int bigtub[176] = {
		456, 236,
		471, 160,
		471, 127,
		463, 94,
		442, 24,
		428, 11,
		410, 8,
		38, 21,
		26, 27,
		19, 37,
		16, 49,
		16, 101,
		20, 109,
		25, 116,
		36, 121,
		50, 125,
		90, 129,
		120, 132,
		131, 134,
		136, 142,
		136, 161,
		110, 165,
		99, 169,
		89, 179,
		84, 190,
		84, 594,
		82, 599,
		73, 606,
		52, 620,
		44, 626,
		53, 640,
		87, 622,
		95, 615,
		101, 606,
		103, 193,
		107, 189,
		122, 186,
		205, 170,
		256, 169,
		322, 173,
		331, 177,
		336, 182,
		341, 189,
		342, 200,
		341, 209,
		337, 216,
		334, 229,
		334, 243,
		390, 240,
		389, 209,
		382, 185,
		376, 175,
		368, 166,
		357, 158,
		341, 152,
		315, 148,
		289, 146,
		255, 146,
		234, 146,
		205, 150,
		165, 157,
		160, 157,
		158, 153,
		158, 138,
		156, 132,
		149, 124,
		141, 119,
		115, 114,
		83, 110,
		50, 106,
		41, 105,
		36, 102,
		35, 96,
		35, 50,
		37, 45,
		42, 43,
		150, 40,
		238, 38,
		418, 35,
		422, 38,
		423, 46,
		444, 130,
		445, 136,
		445, 146,
		442, 159,
		422, 189,
		407, 205,
		400, 213
	};

	int smalltub[64] = {
		198, 247,
		198, 224,
		203, 216,
		211, 211,
		237, 205,
		257, 204,
		442, 236,
		445, 239,
		445, 280,
		448, 611,
		446, 615,
		396, 645,
		388, 651,
		398, 667,
		412, 657,
		456, 635,
		461, 629,
		465, 622,
		465, 610,
		465, 248,
		464, 239,
		461, 230,
		454, 223,
		444, 218,
		269, 186,
		246, 184,
		224, 185,
		201, 190,
		187, 196,
		173, 207,
		162, 222,
		154, 255
	};

	int upperleft[50] = {
		32, 170,
		32, 428,
		35, 430,
		38, 428,
		38, 318,
		46, 314,
		54, 314,
		58, 317,
		66, 333,
		69, 339,
		78, 343,
		89, 347,
		93, 343,
		91, 337,
		80, 333,
		74, 330,
		50, 282,
		53, 279,
		54, 275,
		52, 270,
		46, 270,
		39, 252,
		38, 169,
		35, 168,
		33, 169
	};

	//chains
	map.add(App->physics->CreateChain(0, 0, Map_Pinball, 120, false, 0, MAP));
	map.add(App->physics->CreateChain(0, 0, down2left, 32, false, 0, MAP));
	map.add(App->physics->CreateChain(0, 0, downleft, 24, false, 1, MAP));
	map.add(App->physics->CreateChain(0, 0, right2left, 32, false, 0, MAP));
	map.add(App->physics->CreateChain(0, 0, rightleft, 26, false, 1, MAP));
	map.add(App->physics->CreateChain(0, 0, bigtub, 176, false, 0, BIGTUB));
	map.add(App->physics->CreateChain(0, 0, smalltub, 64, false, 0, SMALLTUB));
	map.add(App->physics->CreateChain(0, 0, upperleft, 50, false, 0, MAP));

	//sensors
	App->physics->CreateRectangleSensor(439, 202, 20, 20, BIGTUB_SENSOR);
	App->physics->CreateRectangleSensor(65,  622, 20, 10, BIGTUB_SENSOR_END);
	App->physics->CreateRectangleSensor(179, 246, 20, 40, SMALLTUB_SENSOR);
	App->physics->CreateRectangleSensor(403, 650, 20, 10, SMALLTUB_SENSOR_END);
	App->physics->CreateRectangleSensor(216, 274, 18, 18, LIGHT1_SENSOR);
	App->physics->CreateRectangleSensor(244, 274, 18, 18, LIGHT2_SENSOR);
	App->physics->CreateRectangleSensor(272, 274, 18, 18, LIGHT3_SENSOR);
	App->physics->CreateRectangleSensor(300, 274, 18, 18, LIGHT4_SENSOR);
	App->physics->CreateRectangleSensor(226, 910, 79, 10, DEAD_SENSOR);

	return true;
}
