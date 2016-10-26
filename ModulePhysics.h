#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum collider
{
	BALL = 0x0001,
	BIGTUB = 0x0002,
	SMALLTUB = 0x0004,
	MAP = 0x0008,
	BIGTUB_SENSOR = 0x0010,
	BIGTUB_SENSOR_END = 0x0020,
	SMALLTUB_SENSOR = 0x0040,
	SMALLTUB_SENSOR_END = 0x0080,
	LAUNCHER_SENSOR = 0x0100,
	LIGHT1_SENSOR = 0X200,
	LIGHT2_SENSOR = 0X800,
	LIGHT3_SENSOR = 0X1000,
	LIGHT4_SENSOR = 0X2000,
	LIGHT5_SENSOR = 0X10000,
	LIGHT6_SENSOR = 0X20000,
	LIGHT7_SENSOR = 0X40000,
	LIGHT8_SENSOR = 0X80000,
	PISTON = 0X100000,
	BOUNCER = 0X200000,
	NONE= 0X400000,
	DEAD_SENSOR = 0X800000
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

	PhysBody* CreateCircle(int x, int y, int radius, bool typeBody, collider coll);
	PhysBody * CreateRectangle(int x, int y, int width, int height, bool is_dyn, SDL_Texture * text, collider coll);
	PhysBody* CreateRectangle(int x, int y, int width, int height, bool type, collider coll, SDL_Texture* text = nullptr);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, collider coll);
	PhysBody* CreateChain(int x, int y, int* points, int size, bool is_dyn, int rest, collider coll);
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