#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleTextures.h"
#include "p2Point.h"
#include "math.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModulePlayer::~ModulePlayer()
{}


// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	/*LeftFlipper.graphic = App->textures->Load("pinball/RightFlipper.png");
	RightFlipper.graphic = App->textures->Load("pinball/LeftFlipper.png");
	//flipper1.fx = flipper2.fx = App->audio->LoadFx("pinball/flipper.wav");

	// Pivot 0, 0
	int f1[16] = {
		0, 18,
		4, 5,
		15, 0,
		91, 9,
		96, 18,
		89, 27,
		13, 34,
		5, 30
	};

	LeftFlipper.body = App->physics->AddBody({ 159, 829, 0, 0 }, f1, 16, b_dynamic);
	LeftFlipper_wheel = App->physics->AddBody(132 + 14, 829 + 17, 10, b_static);
	App->physics->CreateRevoluteJoint(LeftFlipper.body, LeftFlipper_wheel, 14, 17, 0, 0, 30, -30);

	// Pivot 0, 0
	int f2[16] = {
		81, 0,
		6, 8,
		0, 17,
		6, 26,
		83, 35,
		91, 28,
		95, 17,
		91, 5
	};

	RightFlipper.body = App->physics->AddBody({ 273, 920, 0, 0 }, f2, 16, b_dynamic);
	RightFlipper_wheel = App->physics->AddBody(273 + 80, 920 + 17, 10, b_static);
	App->physics->CreateRevoluteJoint(RightFlipper.body, RightFlipper_wheel, 80, 17, 0, 0, 30, -30);
	*/

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	/*App->textures->Unload(RightFlipper.graphic);
	App->textures->Unload(LeftFlipper.graphic);*/

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	/*if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		//App->audio->PlayFx(flipper.fx);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		LeftFlipper.body->Turn(-360);
		
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		RightFlipper.body->Turn(360);
		
	}
	int x, y;

	LeftPlayer.body->GetPosition(x, y);
	App->renderer->Blit(LeftPlayer.graphic, x, y, NULL, 1.0f, LeftPlayer.body->GetAngle(), 0, 0);

	RightPlayer.body->GetPosition(x, y);
	App->renderer->Blit(RightPlayer.graphic, x, y, NULL, 1.0f, RightPlayer.body->GetAngle(), 0, 0);
	*/

	return UPDATE_CONTINUE;
}

