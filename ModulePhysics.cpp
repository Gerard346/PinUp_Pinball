#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleSceneIntro.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");
	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);
	
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, bool is_dyn, collider coll, int rest, category cat)
{
	b2BodyDef body;
	if (is_dyn)
		body.type = b2_dynamicBody;
	else
		body.type = b2_staticBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.filter.categoryBits = coll;
	fixture.density = 1.0f;
	fixture.restitution = rest;
	fixture.filter.maskBits = MAP | BIGTUB_SENSOR | SMALLTUB_SENSOR | LEVER | PISTON | SENSOR | BOUNCER | BALL | BIGTUB_SENSOR2;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;
	pbody->listener = App->scene_intro;
	pbody->category = cat;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, bool is_dyn, collider coll, SDL_Texture* text)
{
	b2BodyDef body;
	if (is_dyn)
		body.type = b2_dynamicBody;
	else
		body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = 0.0;
	fixture.filter.categoryBits = coll;
	fixture.filter.maskBits = BALL | PISTON;
	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;

	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	pbody->texture = text;
	return pbody;
}


PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, collider coll, category cat)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;
	fixture.filter.categoryBits = coll;
	fixture.filter.maskBits = BALL;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->category = cat;

	return pbody;
}

PhysBody * ModulePhysics::CreatePolygon(int x, int y, int* points, int size, float res, bool isdyn, collider coll, bool is_sensor, category cat)
{
	b2BodyDef body;
	if (isdyn)
		body.type = b2_dynamicBody;
	else
		body.type = b2_staticBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2Vec2* vertices = new b2Vec2[size / 2];
	for (uint i = 0; i < size / 2; ++i)
	{
		vertices[i].Set(PIXEL_TO_METERS(points[i * 2 + 0]), PIXEL_TO_METERS(points[i * 2 + 1]));
	}

	b2PolygonShape polygonShape;
	polygonShape.Set(vertices, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &polygonShape;
	fixture.density = 1.0f;

	if (is_sensor == true)
	{
		fixture.isSensor = true;
	}

	else
	{
		fixture.restitution = res;
	}

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	pbody->category= cat;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, bool is_dyn, int rest, collider coll, category cat)
{
	b2BodyDef body;
	if (is_dyn)
		body.type = b2_dynamicBody;
	else
		body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.restitution = rest;
	fixture.filter.categoryBits = coll;
	fixture.filter.maskBits = BALL;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	pbody->category = cat;

	return pbody;
}

b2RevoluteJoint* ModulePhysics::CreateRevolutionJoint(PhysBody* bodyA, PhysBody* bodyB, float localAnchorA_x, float localAnchorA_y, float localAnchorB_x, float localAnchorB_y, int reference_angle, int upper_angle, int lower_angle)
{
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = bodyA->body;
	revoluteJointDef.bodyB = bodyB->body;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(localAnchorA_x, localAnchorA_y);
	revoluteJointDef.localAnchorB.Set(localAnchorB_x, localAnchorB_y);
	revoluteJointDef.referenceAngle = reference_angle;
	revoluteJointDef.enableLimit = true;
	revoluteJointDef.lowerAngle = lower_angle*DEGTORAD;
	revoluteJointDef.upperAngle = upper_angle*DEGTORAD;

	b2RevoluteJoint* rev_joint = (b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);
	return rev_joint;
}

//Piston
void ModulePhysics::CreatePrismaticJoint(PhysBody* bodya, PhysBody* bodyb)

{
	b2PrismaticJointDef prismaticJointDef;
	prismaticJointDef.bodyA = bodya->body;
	prismaticJointDef.bodyB = bodyb->body;
	prismaticJointDef.collideConnected = true;


	prismaticJointDef.localAnchorA.Set(0, 0);
	prismaticJointDef.localAnchorB.Set(0, -1);
	prismaticJointDef.localAxisA.Set(0, -1);
	prismaticJointDef.enableLimit = true;
	prismaticJointDef.lowerTranslation = -0.02;
	prismaticJointDef.upperTranslation = 1.0;
	(b2PrismaticJoint*)world->CreateJoint(&prismaticJointDef);

}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}
			b2Vec2 point(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));

			// TODO 1: If mouse button 1 is pressed ...
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				// test if the current body contains mouse position
				bool hit;
				b2Transform transform = b->GetTransform();
				b2Shape* shape = f->GetShape();
				hit = shape->TestPoint(transform, point);


				if (hit == true)
				{
					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = b;
					def.target = point;
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * b->GetMass();
					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
				}
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT  && mouse_joint)
			{
				mouse_joint->SetTarget(point);
				App->renderer->DrawLine(METERS_TO_PIXELS(point.x), METERS_TO_PIXELS(point.y), METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), 255, 255, 255);
			}
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP  && mouse_joint)
			{
				world->DestroyJoint(mouse_joint);
				mouse_joint = nullptr;
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!

	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void ModulePhysics::sensor_collision(PhysBody* bodyA, PhysBody* bodyB)
{
	b2Filter filter = bodyA->body->GetFixtureList()->GetFilterData();

	switch (bodyB->body->GetFixtureList()->GetFilterData().categoryBits)
	{
	case BIGTUB_SENSOR:
		App->audio->PlayFx(App->scene_intro->ramp_fx, 0);
		filter.maskBits = BIGTUB | BIGTUB_SENSOR_END | LEVER | PISTON;
		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;
	case BIGTUB_SENSOR2:
		App->audio->PlayFx(App->scene_intro->ramp_fx, 0);
		filter.maskBits = BIGTUB | BIGTUB_SENSOR_END | LEVER | PISTON | BOUNCER;
		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;
	case BIGTUB_SENSOR_END:
		filter.maskBits = MAP | BIGTUB_SENSOR | SMALLTUB_SENSOR | LEVER | PISTON | SENSOR | BIGTUB_SENSOR2;
		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;
	case SMALLTUB_SENSOR:
		App->audio->PlayFx(App->scene_intro->ramp_fx, 0);
		filter.maskBits = SMALLTUB | SMALLTUB_SENSOR_END | LEVER | PISTON;
		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;
	case SMALLTUB_SENSOR_END:
		filter.maskBits = MAP | BIGTUB_SENSOR | SMALLTUB_SENSOR | LEVER | PISTON | SENSOR;
		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;
	case SENSOR:
		filter.maskBits = LEVER;
		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;
	}
}
