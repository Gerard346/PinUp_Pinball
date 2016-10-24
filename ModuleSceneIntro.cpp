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

	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	lever_fx = App->audio->LoadFx("pinball/lever.wav");

	circle = App->textures->Load("pinball/Ball.png"); 
	box = App->textures->Load("pinball/crate.png");
	map_texture = App->textures->Load("pinball/Map_Pinball.png");
	Lever_L = App->textures->Load("pinball/LeftFlipper.png");
	Lever_R = App->textures->Load("pinball/RightFlipper.png");

	CircleLever_L = App->physics->CreateCircle(167, 837, 7, 0);
	BodyLever_L = App->physics->CreateRectangle(192, 837, 45, 12, 1, Lever_L);
	PivotLever_L = App->physics->CreateCircle(192, 837, 6, 1);

	App->physics->CreateRevolutionJoint(BodyLever_L, CircleLever_L, -0.5, 0, 0, 0, 0, 25, -20);
	App->physics->CreateRevolutionJoint(BodyLever_L, PivotLever_L, 0.35, 0, 0, 0, 0, 25, -20);

	CircleLever_R = App->physics->CreateCircle(286, 837, 7, 0);
	BodyLever_R = App->physics->CreateRectangle(261, 837, 45, 12, 1, Lever_R);
	PivotLever_R = App->physics->CreateCircle(261, 837, 6, 1);

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
	App->renderer->Blit(Lever_L, x - 7, y - 14, NULL, 1.0f, BodyLever_L->GetRotation()-6);

	BodyLever_R->GetPosition(x, y);
	App->renderer->Blit(Lever_R, x + 3, y - 12, NULL, 1.0f, BodyLever_R->GetRotation()+6);

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
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 11, true));
		circles.getLast()->data->listener = this;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	//App->audio->PlayFx(bonus_fx);
	
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}
}


bool ModuleSceneIntro::CreateMap()
{
	int Map_Pinball[116] = {
		467, 899,
		478, 891,
		477, 875,
		478, 479,
		474, 443,
		460, 420,
		447, 410,
		446, 351,
		454, 295,
		452, 238,
		424, 219,
		389, 211,
		340, 209,
		316, 249,
		266, 257,
		220, 250,
		184, 247,
		156, 249,
		145, 273,
		131, 295,
		114, 320,
		108, 313,
		138, 271,
		146, 252,
		146, 224,
		144, 145,
		30, 144,
		12, 153,
		5, 175,
		5, 209,
		5, 477,
		10, 486,
		18, 491,
		30, 486,
		31, 473,
		60, 455,
		81, 482,
		38, 522,
		39, 560,
		6, 582,
		7, 787,
		10, 798,
		27, 812,
		189, 887,
		190, 905,
		265, 907,
		265, 887,
		434, 807,
		446, 796,
		450, 767,
		450, 597,
		417, 560,
		417, 522,
		447, 494,
		450, 477,
		454, 487,
		453, 887,
		461, 897
	};

	map.add(App->physics->CreateChain(0, 0, Map_Pinball, 116, false));
	
	return true;
}