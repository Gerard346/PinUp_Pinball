#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"


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
	collide_fx = App->audio->LoadFx("pinball/Fx/collide.wav");
	main_song_fx = App->audio->LoadFx("pinball/Fx/Main_Song.wav");
	App->audio->PlayFx(main_song_fx);

	circle = App->textures->Load("pinball/Sprites/Ball.png"); 
	box = App->textures->Load("pinball/Sprites/crate.png");
	map_texture = App->textures->Load("pinball/Sprites/Map_Pinball.png");
	Lever_L = App->textures->Load("pinball/Sprites/LeftFlipper.png");
	Lever_R = App->textures->Load("pinball/Sprites/RightFlipper.png");
	piston_texture = App->textures->Load("pinball/Sprites/piston.png");
	map_texture_walls = App->textures->Load("pinball/Sprites/walls.png");
	
	numbers_others = App->textures->Load("pinball/Sprites/Numbers_Others.png");
	numbers_score = App->textures->Load("pinball/Sprites/Numbers_Score.png");
	light = App->textures->Load("pinball/Sprites/Light_Bulb.png");
	title = App->textures->Load("pinball/Sprites/Title_Sprites.png");

	light_bulb = { 0,0,18,17 };

	//App->physics->CreatePrismaticJoint(0,0, 0, 0, 0, 0, 0, 0, 0);
	//App->physics->CreatePrismaticJoint(0, 0, 0, 0, 0, 0, 0, , 0);	

	CircleLever_L = App->physics->CreateCircle(167, 837, 7, 0, LEVER);
	BodyLever_L = App->physics->CreateRectangle(192, 837, 45, 12, 1, LEVER, Lever_L);
	PivotLever_L = App->physics->CreateCircle(192, 837, 6, 1,  LEVER);

	App->physics->CreateRevolutionJoint(BodyLever_L, CircleLever_L, -0.5, 0, 0, 0, 0, 25, -20);
	App->physics->CreateRevolutionJoint(BodyLever_L, PivotLever_L, 0.35, 0, 0, 0, 0, 25, -20);

	CircleLever_R = App->physics->CreateCircle(286, 837, 7, 0, LEVER);
	BodyLever_R = App->physics->CreateRectangle(261, 837, 45, 12, 1, LEVER, Lever_R);
	PivotLever_R = App->physics->CreateCircle(261, 837, 6, 1, LEVER);

	App->physics->CreateRevolutionJoint(BodyLever_R, CircleLever_R, 0.5, 0, 0, 0, 0, 21, -20);
	App->physics->CreateRevolutionJoint(BodyLever_R, PivotLever_R, -0.35, 0, 0, 0, 0, 21, -20);

	Piston = App->physics->CreateRectangle(465, 860, 15, 10, true, PISTON);
	Piston2 = App->physics->CreateRectangle(465, 894, 15, 10, false, PISTON);
	App->physics->CreatePrismaticJoint(Piston, Piston2);

	dead_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT + 10, SCREEN_WIDTH / 2, 10, SENSOR, DEAD_SENSOR);

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
	App->renderer->Blit(Lever_R, x - 1, y - 10, NULL, 1.0f, BodyLever_R->GetRotation()+6);

	int x1, y1;
	Piston->GetPosition(x1, y1);
	App->renderer->Blit(piston_texture, x1+2, y1, NULL, 1.0f);
	

	PivotLever_L->body->ApplyForceToCenter(b2Vec2(0, 10), true);
	PivotLever_R->body->ApplyForceToCenter(b2Vec2(0, 10), true);
	Piston->body->ApplyForceToCenter(b2Vec2(0, 10), true);
	
	//flippers
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (fx_lever_left == false)
		{
			App->audio->PlayFx(lever_fx, 0);
			fx_lever_left = true;
		}
		PivotLever_L->body->ApplyForceToCenter(b2Vec2(0, -110), true);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (fx_lever_right == false)
		{
			App->audio->PlayFx(lever_fx, 0);
			fx_lever_right = true;
		}
		PivotLever_R->body->ApplyForceToCenter(b2Vec2(0, -110), true);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		fx_lever_right = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		fx_lever_left = false;
	}

	//ball
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 8, true, BALL));
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN && spawned == false)
	{

		App->audio->PlayFx(intro_fx, 0);
		circles.add(App->physics->CreateCircle(462, 800, 7, true, BALL));
		circles.getLast()->data->listener = this;
		spawned = true;	
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) && spawned == true)
	{
		circles.getLast()->data->body->ApplyForceToCenter(b2Vec2(0.0f, -160.0f), true);
	}
	if (push == true)
	{
		circles.getLast()->data->body->ApplyForceToCenter(b2Vec2(0.0f, -160.0f), true);
		push = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		fx_intro = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (fx_intro == false)
		{
			App->audio->PlayFx(throw_fx, 0);
			fx_intro = true;
		}
		Piston->body->ApplyForceToCenter(b2Vec2(0.1f, 0.01f), true);	
	}
	else 
	{
		Piston->body->ApplyForceToCenter(b2Vec2(0.0f, -30.0f), true);
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

	if (light1 == true) 
	{
		if (l1 == false) 
		{
			App->audio->PlayFx(bulb_fx, 0);
			App->player->score += 11 * App->player->multiplier;
			l1 = true;
		}
		App->renderer->Blit(light, 208, 266, &light_bulb);
	}
	 if (light2 == true) 
	{
		if (l2 == false) 
		{
			App->audio->PlayFx(bulb_fx, 0);
			App->player->score += 11 * App->player->multiplier;
			l2 = true;
		}
		App->renderer->Blit(light, 236, 266, &light_bulb);
	}
	 if (light3 == true) 
	{
		if (l3 == false) 
		{
			App->audio->PlayFx(bulb_fx, 0);
			App->player->score += 11 * App->player->multiplier;
			l3 = true;
		}
		App->renderer->Blit(light, 264, 266, &light_bulb);
	}
	 if (light4 == true) 
	{
		if (l4 == false)
		{
			App->audio->PlayFx(bulb_fx, 0);
			App->player->score += 11 * App->player->multiplier;
			l4 = true;
		}
		App->renderer->Blit(light, 292, 266, &light_bulb);
	}
	if (light5 == true)
	 {
		 if (l5 == false)
		 {
			 App->audio->PlayFx(bulb_fx, 0);
			 App->player->score += 11 * App->player->multiplier;
			 l5 = true;
		 }
		 App->renderer->Blit(light, 47, 531, &light_bulb);
	 }
	 if (light6 == true)
	 {
		 if (l6 == false)
		 {
			 App->audio->PlayFx(bulb_fx, 0);
			 App->player->score += 11 * App->player->multiplier;
			 l6 = true;
		 }
		 App->renderer->Blit(light, 42, 644, &light_bulb);
	 }
	 if (light7 == true)
	 {
		 if (l7 == false)
		 {
			 App->audio->PlayFx(bulb_fx, 0);
			 App->player->score += 11 * App->player->multiplier;
			 l7 = true;
		 }
		 App->renderer->Blit(light, 386, 532, &light_bulb);
	 }
	 if (light8 == true)
	 {
		 if (l8 == false)
		 {
			 App->audio->PlayFx(bulb_fx, 0);
			 App->player->score += 11 * App->player->multiplier;
			 l8 = true;
		 }
		 App->renderer->Blit(light, 40, 148, &light_bulb);
	 }
	 if (light9 == true)
	 {
		 if (l9 == false)
		 {
			 App->audio->PlayFx(bulb_fx, 0);
			 App->player->score += 11 * App->player->multiplier;
			 l9 = true;
		 }
		 App->renderer->Blit(light, 68, 148, &light_bulb);
	 }
	 if (light10 == true)
	 {
		 if (l10 == false)
		 {
			 App->audio->PlayFx(bulb_fx, 0);
			 App->player->score += 11 * App->player->multiplier;
			 l10= true;
		 }
		 App->renderer->Blit(light, 96, 148, &light_bulb);
	 }


	if (light1 == true && light2 == true && light3 == true && light4 == true)
	{
		light1 = false;
		light2 = false;
		light3 = false;
		light4 = false;
		l1 = false;
		l2 = false;
		l3 = false;
		l4 = false;
		App->player->multiplier += 1;
	}
	
	
	if (dead == true) 
	{
		App->audio->PlayFx(dead_fx, 0);
		if (App->player->lives > 1) 
		{
			dead = false;
			App->player->lives-=1;
			App->player->multiplier = 1;
			spawned = false;
		

		}
		else
		{
			App->player->RestartGame();
		}
	}
	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyA != nullptr)
	{
		if (bodyB->category == CHAIN_SENSOR)
		{
			App->physics->sensor_collision(bodyA, bodyB);
		}
		if (bodyB->category == LIGHT1_SENSOR)
		{
			App->scene_intro->light1 = true;
		}
		if (bodyB->category == LIGHT2_SENSOR)
		{
			App->scene_intro->light2 = true;
		}
		if (bodyB->category == LIGHT3_SENSOR)
		{
			App->scene_intro->light3 = true;
		}
		if (bodyB->category == LIGHT4_SENSOR)
		{
			App->scene_intro->light4 = true;
		}
		if (bodyB->category == LIGHT5_SENSOR)
		{
			App->scene_intro->light5 = true;
		}
		if (bodyB->category == LIGHT6_SENSOR)
		{
			App->scene_intro->light6 = true;
		}
		if (bodyB->category == LIGHT7_SENSOR)
		{
			App->scene_intro->light7 = true;
		}
		if (bodyB->category == LIGHT8_SENSOR)
		{
			App->scene_intro->light8 = true;
		}
		if (bodyB->category == LIGHT9_SENSOR)
		{
			App->scene_intro->light9 = true;
		}
		if (bodyB->category == LIGHT10_SENSOR)
		{
			App->scene_intro->light10 = true;
		}
		if (bodyB->category == PUSH_SENSOR)
		{
			push = true;
		}
		if (bodyB->category == DEAD_SENSOR)
		{
			App->scene_intro->dead = true;
		}
		if (bodyB->category == LEFT_SENSOR)
		{
			App->player->score += 11;
		}
		if (bodyB->category == RIGHT_SENSOR)
		{
			App->player->score += 11;
		}
		if (bodyB->category == SU1_SENSOR)
		{
			App->player->score += 11;
		}
		if (bodyB->category == SU2_SENSOR)
		{
			App->player->score += 11;
		}
		if (bodyB->category == BUTTONR_SENSOR)
		{
			App->audio->PlayFx(bonus_fx, 0);
			App->player->score += 50;
		}
		if (bodyB->category == BUTTONL_SENSOR)
		{
			App->audio->PlayFx(bonus_fx, 0);
			App->player->score += 50;
		}

	}
}

bool ModuleSceneIntro::CreateMap()
{
	int Map_Pinball[134] = {
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
		149, 232,
		149, 243,
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
		154, 253,
		158, 233,
		151, 220,
		151, 174,
		151, 141,
		400, 141,
		476, 141,
		455, 240
	};

	int topwall[20] = {
		336, 222,
		327, 238,
		318, 250,
		309, 254,
		282, 257,
		263, 259,
		236, 257,
		211, 253,
		203, 249,
		334, 222
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

	int bigtub[110] = {
		454, 246,
		455, 228,
		469, 168,
		471, 154,
		471, 130,
		470, 123,
		445, 29,
		442, 23,
		437, 17,
		429, 12,
		421, 9,
		413, 9,
		40, 21,
		29, 25,
		23, 32,
		19, 39,
		16, 48,
		16, 102,
		21, 112,
		30, 119,
		40, 124,
		126, 133,
		131, 135,
		135, 141,
		135, 160,
		131, 162,
		115, 164,
		100, 168,
		93, 173,
		88, 179,
		85, 188,
		84, 195,
		83, 595,
		81, 599,
		69, 608,
		51, 620,
		49, 618,
		76, 599,
		80, 593,
		81, 190,
		86, 177,
		99, 165,
		132, 159,
		132, 144,
		124, 137,
		32, 126,
		12, 111,
		13, 29,
		29, 15,
		436, 4,
		461, 53,
		478, 121,
		478, 155,
		460, 229,
		458, 247
	};

	int bigtub2[72] = {
		393, 213,
		398, 213,
		424, 187,
		437, 167,
		442, 157,
		445, 141,
		445, 133,
		421, 38,
		418, 36,
		43, 44,
		39, 45,
		36, 49,
		36, 99,
		38, 103,
		43, 105,
		135, 117,
		143, 120,
		150, 124,
		156, 131,
		159, 140,
		159, 153,
		161, 156,
		164, 157,
		195, 152,
		226, 147,
		250, 145,
		269, 145,
		303, 147,
		328, 149,
		345, 153,
		363, 162,
		376, 175,
		382, 184,
		386, 195,
		389, 209,
		392, 213
	};

	int bigtub3[62] = {
		336, 224,
		341, 209,
		342, 202,
		342, 192,
		339, 185,
		332, 177,
		324, 174,
		288, 170,
		235, 170,
		202, 171,
		107, 190,
		103, 193,
		102, 600,
		100, 608,
		96, 615,
		89, 621,
		62, 635,
		61, 637,
		62, 638,
		84, 628,
		93, 622,
		100, 614,
		106, 602,
		106, 195,
		109, 192,
		203, 174,
		321, 176,
		333, 183,
		338, 193,
		332, 216,
		332, 223
	};

	int smalltub[60] = {
		155, 249,
		158, 232,
		167, 214,
		181, 199,
		203, 188,
		238, 182,
		257, 183,
		442, 216,
		453, 221,
		461, 229,
		465, 239,
		466, 249,
		466, 614,
		464, 624,
		461, 631,
		452, 638,
		413, 657,
		414, 660,
		450, 642,
		461, 634,
		469, 618,
		469, 240,
		461, 223,
		440, 212,
		255, 178,
		210, 181,
		170, 201,
		154, 225,
		150, 252,
		154, 252
	};

	int smalltub2[44] = {
		202, 251,
		199, 249,
		199, 224,
		201, 219,
		208, 212,
		235, 206,
		253, 204,
		441, 236,
		444, 239,
		447, 609,
		445, 615,
		435, 623,
		395, 646,
		394, 643,
		437, 619,
		443, 614,
		445, 609,
		442, 240,
		253, 208,
		209, 216,
		202, 224,
		202, 249
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
	int blueL[16] = {
		111, 233,
		106, 243,
		107, 252,
		117, 257,
		126, 252,
		129, 243,
		123, 235,
		111, 233
	};

	int redLU[16] = {
		69, 225,
		78, 229,
		82, 238,
		76, 248,
		64, 248,
		58, 240,
		60, 230,
		69, 225
	};
	int redLD[16] = {
		85, 270,
		76, 278,
		78, 287,
		88, 292,
		98, 288,
		100, 280,
		95, 272,
		85, 270
	};
	int blueR[16] = {
		457, 162,
		448, 161,
		441, 168,
		441, 178,
		450, 185,
		461, 181,
		464, 169,
		457, 162
	};
	
	int smallwall[14] = {
		66, 175,
		64, 173,
		61, 174,
		61, 199,
		64, 200,
		66, 198,
		66, 176
	};

	int smallwall2[14] = {
		91, 174,
		89, 175,
		89, 198,
		91, 201,
		93, 198,
		93, 177,
		92, 174
	};

	int smallwall3[14] = {
		117, 176,
		118, 174,
		121, 175,
		121, 198,
		119, 200,
		117, 199,
		117, 178
	};

	//chains
	map.add(App->physics->CreateChain(0, 0, Map_Pinball, 134, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, topwall, 20, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, down2left, 32, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, downleft, 24, false, 1, MAP));
	map.add(App->physics->CreateChain(0, 0, right2left, 32, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, rightleft, 26, false, 1, MAP));
	map.add(App->physics->CreateChain(0, 0, bigtub, 110, false, 0, BIGTUB));
	map.add(App->physics->CreateChain(0, 0, bigtub2, 72, false, 0, BIGTUB));
	map.add(App->physics->CreateChain(0, 0, bigtub3, 60, false, 0, BIGTUB));
	map.add(App->physics->CreateChain(0, 0, smalltub, 60, false, 0, SMALLTUB));
	map.add(App->physics->CreateChain(0, 0, smalltub2, 44, false, 0, SMALLTUB));
	map.add(App->physics->CreateChain(0, 0, upperleft, 50, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, smallwall, 14, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, smallwall2, 14, false, 0.2, MAP));
	map.add(App->physics->CreateChain(0, 0, smallwall3, 14, false, 0.2, MAP));

	//sensors
	App->physics->CreateRectangleSensor(430, 212, 50, 20, BIGTUB_SENSOR, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(362, 212, 50, 20, BIGTUB_SENSOR, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(362, 234, 50, 10, BIGTUB_SENSOR_END, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(430, 234, 50, 10, BIGTUB_SENSOR_END, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(65, 622, 20, 10, BIGTUB_SENSOR_END, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(179, 244, 48, 25, SMALLTUB_SENSOR, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(179, 286, 50, 10, SMALLTUB_SENSOR_END, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(395, 650, 20, 10, SMALLTUB_SENSOR_END, CHAIN_SENSOR);
	App->physics->CreateRectangleSensor(216, 251, 18, 18, SENSOR, LIGHT1_SENSOR);
	App->physics->CreateRectangleSensor(244, 254, 18, 18, SENSOR, LIGHT2_SENSOR);
	App->physics->CreateRectangleSensor(272, 254, 18, 18, SENSOR, LIGHT3_SENSOR);
	App->physics->CreateRectangleSensor(300, 251, 18, 18, SENSOR, LIGHT4_SENSOR);
	App->physics->CreateRectangleSensor(57, 540, 18, 18, SENSOR, LIGHT5_SENSOR);
	App->physics->CreateRectangleSensor(52, 654, 18, 18, SENSOR, LIGHT6_SENSOR);
	App->physics->CreateRectangleSensor(396, 540, 18, 18, SENSOR, LIGHT7_SENSOR);
	App->physics->CreateRectangleSensor(49, 157, 18, 18, SENSOR, LIGHT8_SENSOR);
	App->physics->CreateRectangleSensor(78, 157, 18, 18, SENSOR, LIGHT9_SENSOR);
	App->physics->CreateRectangleSensor(106, 157, 18, 18, SENSOR, LIGHT10_SENSOR);
	App->physics->CreateRectangleSensor(226, 900, 79, 20, SENSOR, DEAD_SENSOR);
	App->physics->CreateRectangleSensor(20, 490, 10, 10, SENSOR, PUSH_SENSOR);
	App->physics->CreateRectangleSensor(51, 682, 9, 24, SENSOR, LEFT_SENSOR);
	App->physics->CreateRectangleSensor(400, 682, 9, 24, SENSOR, RIGHT_SENSOR);
	App->physics->CreateRectangleSensor(49, 186, 9, 24, SENSOR, SU1_SENSOR);
	App->physics->CreateRectangleSensor(77, 186, 9, 24, SENSOR, SU2_SENSOR);
	App->physics->CreateRectangleSensor(41, 540, 6, 17, SENSOR, BUTTONR_SENSOR);
	App->physics->CreateRectangleSensor(411, 540, 6, 17, SENSOR, BUTTONL_SENSOR);

	//bouncers
	//top left
	App->physics->CreateCircle(69, 238, 10, false, BOUNCER, 3);
	App->physics->CreateCircle(88, 281, 10, false, BOUNCER, 3);
	App->physics->CreateCircle(116, 244, 10, false, BOUNCER, 3);

	return true;
}
