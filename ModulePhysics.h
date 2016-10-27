#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -10.5f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum category
{
	CHAIN_SENSOR = 1,
	LIGHT1_SENSOR,
	LIGHT2_SENSOR,
	LIGHT3_SENSOR,
	LIGHT4_SENSOR,
	LIGHT5_SENSOR,
	LIGHT6_SENSOR,
	LIGHT7_SENSOR,
	LIGHT8_SENSOR,
	LIGHT9_SENSOR,
	LIGHT10_SENSOR,
	PUSH_SENSOR,
	DEAD_SENSOR,
	LEFT_SENSOR,
	RIGHT_SENSOR,
	SU1_SENSOR,
	SU2_SENSOR,
	BUTTONL_SENSOR,
	BUTTONR_SENSOR

};

enum collider
{
	BALL = 1,
	BIGTUB = 2,
	SMALLTUB = 4,
	MAP = 8,
	BIGTUB_SENSOR =  16,
	BIGTUB_SENSOR_END = 32,
	SMALLTUB_SENSOR = 64,
	SMALLTUB_SENSOR_END = 128,
	LEVER = 256,
	PISTON = 512,
	BOUNCER = 1024,
	SENSOR = 2048,
	NONE = 0
};


// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
	SDL_Texture* texture = nullptr;
	category category;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, bool typeBody, collider coll, int rest = 0);
	PhysBody* CreateRectangle(int x, int y, int width, int height, bool type, collider coll, SDL_Texture* text = nullptr);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, collider coll, category cat);
	PhysBody* CreateChain(int x, int y, int* points, int size, bool is_dyn, int rest, collider coll);
	PhysBody* CreatePolygon(int x, int y, int* points, int size, float res, bool isdyn, collider coll, bool is_sensor, category cat);
	b2RevoluteJoint* CreateRevolutionJoint(PhysBody* bodyA, PhysBody* bodyB, float localAnchorA_x, float localAnchorA_y, float localAnchorB_x, float localAnchorB_y, int reference_angle, int upper_angle, int lower_angle);
	void CreatePrismaticJoint(PhysBody* bodya, PhysBody* bodyb);
	void sensor_collision(PhysBody* bodyA, PhysBody* bodyB);
	
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
	b2Fixture* temp = NULL;
	p2List<PhysBody*> bodies;

};