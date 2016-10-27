#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "p2Point.h"
#include "math.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	lives = 3;
	score = 0;
	multiplier = 1;

}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	sprintf_s(title, "Multiplier: %02d-Lives: %02d-Score: %08d  ", multiplier, lives, score);
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModulePlayer::RestartGame() {

	lives = 3;
	score = 0;
	multiplier = 1;
	App->scene_intro->spawned = false;
	App->scene_intro->dead = false;
}


