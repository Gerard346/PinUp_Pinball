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
	lives = 6;
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

	lives = 6;
	score = 0;
	multiplier = 1;
	App->scene_intro->spawned = false;
	App->scene_intro->dead = false;
	App->scene_intro->light1 = false;
	App->scene_intro->light2 = false;
	App->scene_intro->light3 = false;
	App->scene_intro->light4 = false;
	App->scene_intro->light5 = false;
	App->scene_intro->light6 = false;
	App->scene_intro->light7 = false;
	App->scene_intro->light8 = false;

	App->scene_intro->su1 = false;
	App->scene_intro->su2 = false;
	App->scene_intro->su3 = false;
	App->scene_intro->su4 = false;

	App->scene_intro->l1 = false;
	App->scene_intro->l2 = false;
	App->scene_intro->l3 = false;
	App->scene_intro->l4 = false;
	App->scene_intro->l5 = false;
	App->scene_intro->l6 = false;
	App->scene_intro->l7 = false;
	App->scene_intro->l8 = false;
	App->scene_intro->l9 = false;
	App->scene_intro->l10 = false;

	App->scene_intro->ls = false;
	App->scene_intro->rs = false;
	App->scene_intro->leftsensor = false;
	App->scene_intro->rightsensor = false;

	App->scene_intro->su1 = false;
	App->scene_intro->su2 = false;
	App->scene_intro->su3 = false;
	App->scene_intro->su4 = false;
	App->scene_intro->s1 = false;
	App->scene_intro->s2 = false;
	App->scene_intro->s3 = false;
	App->scene_intro->s4 = false;

	App->scene_intro->rbutton = false;
	App->scene_intro->lbutton = false;
	App->scene_intro->rb = false;
	App->scene_intro->lb = false;
}


