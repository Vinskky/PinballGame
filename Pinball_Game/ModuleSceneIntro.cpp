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
	background = ball = box = rick = NULL;
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
	int x, y;
	App->renderer->camera.x = App->renderer->camera.y = 0;

	background = App->textures->Load("textures/background.png");
	pinballSet = App->textures->Load("textures/pinballAssets2.png");
	ball = App->textures->Load("textures/ball.png");
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	//Sensors//
	//die sensor
	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	//bumpers boost
	bumperLeft = App->physics->CreateRectangleSensor(147, 595, 5, 85);
	bumperLeft->body->SetTransform(bumperLeft->body->GetPosition(), DEGTORAD * (-22));
	bumperRight = App->physics->CreateRectangleSensor(355, 595, 5, 85);
	bumperRight->body->SetTransform(bumperRight->body->GetPosition(), DEGTORAD * (22));
	//circles boost
	boostFireHydrant = App->physics->CreateCircleSensor(208, 222, 23);
	boostHouse = App->physics->CreateCircleSensor(279, 194, 23);
	boostMask = App->physics->CreateCircleSensor(288, 265, 23);
	boostTermometer = App->physics->CreateCircleSensor(203, 365, 23);

	//create flippers
	boxes.add(App->physics->CreateRectangle(190, 710, 60, 18));
	boxes.getLast()->data->listener = this;
	circles.add(App->physics->CreateCircle(185, 710, 5));
	circles.getLast()->data->body->SetType(b2_staticBody);

	boxes.add(App->physics->CreateRectangle(305, 710, 60, 18));
	boxes.getLast()->data->listener = this;
	circles.add(App->physics->CreateCircle(314, 710, 5));
	circles.getLast()->data->body->SetType(b2_staticBody);

	//Create Central Circles
	circles.add(App->physics->CreateCircle(208, 222, 22));
	circles.getLast()->data->body->SetType(b2_staticBody);
	circles.add(App->physics->CreateCircle(279, 194, 22));
	circles.getLast()->data->body->SetType(b2_staticBody);
	circles.add(App->physics->CreateCircle(288, 265, 22));
	circles.getLast()->data->body->SetType(b2_staticBody);
	circles.add(App->physics->CreateCircle(203, 365, 22));
	circles.getLast()->data->body->SetType(b2_staticBody);

	//Upper boxes
	boxes.add(App->physics->CreateRectangle(231, 118, 11, 33));
	boxes.getLast()->data->body->SetType(b2_staticBody);
	boxes.add(App->physics->CreateRectangle(270, 118, 11, 33));
	boxes.getLast()->data->body->SetType(b2_staticBody);

	//Start Spring
	boxes.add(App->physics->CreateRectangle(497, 730, 35, 14));
	boxes.getLast()->data->body->SetType(b2_kinematicBody);

	//ball
	ballList.add(App->physics->CreateCircle(508,706, 11));
	ballList.getLast()->data->listener = this;
	ballList.getLast()->data->body->SetBullet(true);

	//Starting polligons
	//outsidebounds
	polligons.add(App->physics->CreateChain(0, 0, outsideBounds, 112));
	polligons.add(App->physics->CreateChain(0, 0, innerBound, 90));
	polligons.add(App->physics->CreateChain(0, 0, bouncerLeft, 18));
	polligons.add(App->physics->CreateChain(0, 0, bouncerRight, 16));
	polligons.add(App->physics->CreateChain(0, 0, LLeft, 26));
	polligons.add(App->physics->CreateChain(0, 0, LRight, 28));


	//create joints
	b2RevoluteJointDef revJoint_l;
	revJoint_l.bodyA = boxes.getFirst()->data->body;
	revJoint_l.bodyB = circles.getFirst()->data->body;
	revJoint_l.localAnchorA.Set(PIXEL_TO_METERS(-18), PIXEL_TO_METERS(0));
	revJoint_l.localAnchorB.Set(0, 0);
	revJoint_l.lowerAngle = DEGTORAD * (-30);
	revJoint_l.referenceAngle = 0;
	revJoint_l.upperAngle = DEGTORAD * 30;
	revJoint_l.enableLimit = true;
	b2RevoluteJoint* joint_l = (b2RevoluteJoint*)boxes.getFirst()->data->body->GetWorld()->CreateJoint(&revJoint_l);

	b2RevoluteJointDef revJoint_r;
	revJoint_r.bodyA = boxes.getFirst()->next->data->body;
	revJoint_r.bodyB = circles.getFirst()->next->data->body;
	revJoint_r.localAnchorA.Set(PIXEL_TO_METERS(18), PIXEL_TO_METERS(0));
	revJoint_r.localAnchorB.Set(0, 0);
	revJoint_r.lowerAngle = DEGTORAD * (-30);
	revJoint_r.referenceAngle = 0;
	revJoint_r.upperAngle = DEGTORAD * (30);
	revJoint_r.enableLimit = true;
	b2RevoluteJoint* joint_r = (b2RevoluteJoint*)boxes.getFirst()->next->data->body->GetWorld()->CreateJoint(&revJoint_r);

	//SPRITE COORDINATES//
	flipperLeft = { 0, 0, 88, 22 };
	flipperRight = { 102, 0, 88, 22 };
	bgRect = { 0, 0, 533, 798 };
	playerBall = { 0, 0, 24, 24 };
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
	int x, y;
	App->renderer->Blit(background,0,0,&bgRect);

	//Flippers
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		boxes.getFirst()->data->body->ApplyForceToCenter(b2Vec2(0, -500), 1);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		boxes.getFirst()->next->data->body->ApplyForceToCenter(b2Vec2(0, -500), 1);
	}


	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		ballList.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 11));
		ballList.getLast()->data->listener = this;
		ballList.getLast()->data->body->SetBullet(true);
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	//Spring
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN )
	{
		boxes.getLast()->data->body->SetLinearVelocity(b2Vec2(0, 1));
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		boxes.getLast()->data->body->SetLinearVelocity(b2Vec2(0, -20));

	}
	else if (boxes.getLast()->data->body->GetPosition().y > PIXEL_TO_METERS(750) || boxes.getLast()->data->body->GetPosition().y < PIXEL_TO_METERS(730))
	{
		if (boxes.getLast()->data->body->GetPosition().y < PIXEL_TO_METERS(730))
		{
			boxes.getLast()->data->body->SetTransform({ boxes.getLast()->data->body->GetPosition().x, PIXEL_TO_METERS(730) }, boxes.getLast()->data->body->GetAngle());
		}

		boxes.getLast()->data->body->SetLinearVelocity(b2Vec2(0, 0));
	}

	//Draw Flippers
	PhysBody* flipperl = boxes.getFirst()->data;
	flipperl->GetPosition(x, y);
	App->renderer->Blit(pinballSet, x-20, y , &flipperLeft, 1.0f, (flipperl->GetRotation()));
	PhysBody* flipperr = boxes.getFirst()->next->data;
	flipperr->GetPosition(x, y);
	App->renderer->Blit(pinballSet, x-10, y , &flipperRight, 1.0f, (flipperr->GetRotation()));

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = ballList.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(ball, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	/*//c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	//c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}*/

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

	App->audio->PlayFx(bonus_fx);

	if (bodyB == bumperLeft)
	{
		p2List_item<PhysBody*>* c = ballList.getFirst();
		c->data->body->SetLinearVelocity(b2Vec2(22, -22));
	}
	if (bodyB == bumperRight)
	{
		p2List_item<PhysBody*>* c = ballList.getFirst();
		c->data->body->SetLinearVelocity(b2Vec2(-22, -22));
	}
	if (bodyB == boostFireHydrant || bodyB == boostHouse ||
		bodyB == boostMask || bodyB == boostTermometer)
	{
		int x, y;
		p2List_item<PhysBody*>* c = ballList.getFirst();
		c->data->GetPosition(x, y);
		if(y > y - 22) // radius of bouncers
		{
			if (x > x - 22)
			{
				//down right
				c->data->body->SetLinearVelocity(b2Vec2(10, 10));
			}
			else
			{
				//down left
				c->data->body->SetLinearVelocity(b2Vec2(-10, 10));
			}
		}
		else
		{
			if (x > x - 22)
			{
				//up right
				c->data->body->SetLinearVelocity(b2Vec2(10, -10));
			}
			else
			{
				//up left
				c->data->body->SetLinearVelocity(b2Vec2(-10, -10));
			}
		}
	}
	
	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
