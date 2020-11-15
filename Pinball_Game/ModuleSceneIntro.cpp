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
	background = ball = box = NULL;
	ray_on = false;
	sensed = false;
	currentLvl = START;
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
	LifeSaviour = App->textures->Load("textures/lifeSaviours.png");
	ball = App->textures->Load("textures/ball.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	//die sensor
	dieSensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	//bumpers boost sensor
	bumperLeft = App->physics->CreateRectangleSensor(147, 595, 5, 85);
	bumperLeft->body->SetTransform(bumperLeft->body->GetPosition(), DEGTORAD * (-22));
	bumperRight = App->physics->CreateRectangleSensor(355, 595, 5, 85);
	bumperRight->body->SetTransform(bumperRight->body->GetPosition(), DEGTORAD * (22));
	//level changer sensors
	entryLevel = App->physics->CreateRectangleSensor(200, 78, 15, 15);
	entrySlide = App->physics->CreateRectangleSensor(145, 350, 15, 15);
	exitSlide = App->physics->CreateRectangleSensor(396, 546, 15, 15);
	boostSlide = App->physics->CreateRectangleSensor(225, 50, 15, 15);
	//live saviours
	leftLifeSavour = App->physics->CreateRectangleSensor(59, 733, 28, 8);
	rightLifeSavour = App->physics->CreateRectangleSensor(445, 733, 28, 8);

	//create flippers
	boxes.add(App->physics->CreateRectangle(190, 710, 60, 18));
	boxes.getLast()->data->listener = this;
	circles.add(App->physics->CreateCircle(185, 710, 5));
	circles.getLast()->data->body->SetType(b2_staticBody);

	boxes.add(App->physics->CreateRectangle(305, 710, 60, 18));
	boxes.getLast()->data->listener = this;
	circles.add(App->physics->CreateCircle(314, 710, 5));
	circles.getLast()->data->body->SetType(b2_staticBody);

	boxes.add(App->physics->CreateRectangle(440, 298, 60, 9));
	boxes.getLast()->data->listener = this;
	boxes.getLast()->data->body->GetFixtureList()->SetDensity(3.0f);
	circles.add(App->physics->CreateCircle(462, 298, 5));
	circles.getLast()->data->body->SetType(b2_staticBody);

	//Create Central Circles
	circles.add(App->physics->CreateCircle(208, 222, 22));
	circles.getLast()->data->body->SetType(b2_kinematicBody);
	circles.add(App->physics->CreateCircle(279, 194, 22));
	circles.getLast()->data->body->SetType(b2_kinematicBody);
	circles.add(App->physics->CreateCircle(288, 265, 22));
	circles.getLast()->data->body->SetType(b2_kinematicBody);
	circles.add(App->physics->CreateCircle(203, 365, 22));
	circles.getLast()->data->body->SetType(b2_kinematicBody);

	//Upper boxes
	boxes.add(App->physics->CreateRectangle(231, 118, 11, 33));
	boxes.getLast()->data->body->SetType(b2_staticBody);
	boxes.add(App->physics->CreateRectangle(270, 118, 11, 33));
	boxes.getLast()->data->body->SetType(b2_staticBody);

	//Start Spring
	boxes.add(App->physics->CreateRectangle(497, 750, 35, 14));
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
	revJoint_l.localAnchorA.Set(PIXEL_TO_METERS(-22), PIXEL_TO_METERS(0));
	revJoint_l.localAnchorB.Set(0, 0);
	revJoint_l.lowerAngle = DEGTORAD * (-30);
	revJoint_l.referenceAngle = 0;
	revJoint_l.upperAngle = DEGTORAD * 30;
	revJoint_l.enableLimit = true;
	b2RevoluteJoint* joint_l = (b2RevoluteJoint*)boxes.getFirst()->data->body->GetWorld()->CreateJoint(&revJoint_l);

	b2RevoluteJointDef revJoint_r;
	revJoint_r.bodyA = boxes.getFirst()->next->data->body;
	revJoint_r.bodyB = circles.getFirst()->next->data->body;
	revJoint_r.localAnchorA.Set(PIXEL_TO_METERS(22), PIXEL_TO_METERS(0));
	revJoint_r.localAnchorB.Set(0, 0);
	revJoint_r.lowerAngle = DEGTORAD * (-30);
	revJoint_r.referenceAngle = 0;
	revJoint_r.upperAngle = DEGTORAD * (30);
	revJoint_r.enableLimit = true;
	b2RevoluteJoint* joint_r = (b2RevoluteJoint*)boxes.getFirst()->next->data->body->GetWorld()->CreateJoint(&revJoint_r);


	b2RevoluteJointDef revJoint_r_up;
	revJoint_r_up.bodyA = boxes.getFirst()->next->next->data->body;
	revJoint_r_up.bodyB = circles.getFirst()->next->next->data->body;
	revJoint_r_up.localAnchorA.Set(PIXEL_TO_METERS(45), PIXEL_TO_METERS(0));
	revJoint_r_up.localAnchorB.Set(0, 0);
	revJoint_r_up.lowerAngle = DEGTORAD * (-20);
	revJoint_r_up.referenceAngle = 0;
	revJoint_r_up.upperAngle = DEGTORAD * (60);
	revJoint_r_up.enableLimit = true;
	b2RevoluteJoint* joint_r_up = (b2RevoluteJoint*)boxes.getFirst()->next->next->data->body->GetWorld()->CreateJoint(&revJoint_r_up);

	//SPRITE COORDINATES//
	flipperLeft = { 0, 0, 88, 22 };
	flipperRight = { 102, 0, 88, 22 };
	flipperRightUp = { 0, 30, 70, 16 };
	bgRect = { 0, 0, 533, 798 };
	playerBall = { 0, 0, 24, 24 };
	spring = { 84, 37, 27, 62 };
	leftLifeSaviour = { 0, 0, 34, 65 };
	rightLifeSaviour = { 37, 0, 34, 65 };

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
		boxes.getFirst()->next->next->data->body->ApplyForceToCenter(b2Vec2(0, -500), 1);
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
	else if (boxes.getLast()->data->body->GetPosition().y > PIXEL_TO_METERS(770) || boxes.getLast()->data->body->GetPosition().y < PIXEL_TO_METERS(750))
	{
		if (boxes.getLast()->data->body->GetPosition().y < PIXEL_TO_METERS(750))
		{
			boxes.getLast()->data->body->SetTransform({ boxes.getLast()->data->body->GetPosition().x, PIXEL_TO_METERS(750) }, boxes.getLast()->data->body->GetAngle());
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
	PhysBody* flipperrUp = boxes.getFirst()->next->next->data;
	flipperrUp->GetPosition(x, y);
	App->renderer->Blit(pinballSet, x, y-5 , &flipperRightUp, 1.0f, (flipperrUp->GetRotation()));

	//Draw Spring
	PhysBody* springR = boxes.getLast()->data;
	springR->GetPosition(x, y);
	App->renderer->Blit(pinballSet, x+3, y, &spring, 1.0f);
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

	
	if (currentLvl == START)
	{
		int num = polligons.count();
		for (int i = 0; i < num; i++)
		{
			polligons.getLast()->data->body->GetWorld()->DestroyBody(polligons.getLast()->data->body);
			polligons.del(polligons.getLast());
		}
		polligons.add(App->physics->CreateChain(0, 0, outsideBounds, 112));
		polligons.add(App->physics->CreateChain(0, 0, innerBound, 90));
		polligons.add(App->physics->CreateChain(0, 0, bouncerLeft, 18));
		polligons.add(App->physics->CreateChain(0, 0, bouncerRight, 16));
		polligons.add(App->physics->CreateChain(0, 0, LLeft, 26));
		polligons.add(App->physics->CreateChain(0, 0, LRight, 28));
	}
	else if(currentLvl == FLOOR)
	{
		int num = polligons.count();
		for (int i = 0; i < num; i++)
		{
			polligons.getLast()->data->body->GetWorld()->DestroyBody(polligons.getLast()->data->body);
			polligons.del(polligons.getLast());
		}
		polligons.add(App->physics->CreateChain(0, 0, outsideBounds, 112));
		polligons.add(App->physics->CreateChain(0, 0, innerBound, 90));
		polligons.add(App->physics->CreateChain(0, 0, bouncerLeft, 18));
		polligons.add(App->physics->CreateChain(0, 0, bouncerRight, 16));
		polligons.add(App->physics->CreateChain(0, 0, LLeft, 26));
		polligons.add(App->physics->CreateChain(0, 0, LRight, 28));
		polligons.add(App->physics->CreateChain(0, 0, closeEntrance, 14));
		if(lBlock)
			polligons.add(App->physics->CreateChain(0, 0, leftBlock, 8));
		if(rBlock)
			polligons.add(App->physics->CreateChain(0, 0, rightBlock, 8));

	}
	else if (currentLvl == SLIDE)
	{
		int num = polligons.count();
		for (int i = 0; i < num; i++)
		{
			polligons.getLast()->data->body->GetWorld()->DestroyBody(polligons.getLast()->data->body);
			polligons.del(polligons.getLast());
		}
		polligons.add(App->physics->CreateChain(0, 0, slide, 92));
	}

	if (dead)
	{
		ballList.getLast()->data->body->SetLinearVelocity(b2Vec2(0, 0));
		ballList.getLast()->data->body->SetAngularVelocity(0);
		ballList.getLast()->data->body->SetTransform(b2Vec2( PIXEL_TO_METERS(508), PIXEL_TO_METERS(706)), ballList.getLast()->data->GetRotation());
		dead = false;
		lBlock = false;
		rBlock = false;
		currentLvl = START;
	}

	if (lBlock)
		App->renderer->Blit(LifeSaviour,46,667,&leftLifeSaviour);
	if (rBlock)
	App->renderer->Blit(LifeSaviour, 428, 667, &rightLifeSaviour);

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	if (bodyB == bumperLeft)
	{
		p2List_item<PhysBody*>* c = ballList.getFirst();
		c->data->body->SetLinearVelocity(b2Vec2(22, -22));
		App->audio->PlayFx(bonus_fx);
	}
	if (bodyB == bumperRight)
	{
		p2List_item<PhysBody*>* c = ballList.getFirst();
		c->data->body->SetLinearVelocity(b2Vec2(-22, -22));
		App->audio->PlayFx(bonus_fx);
	}

	p2List_item<PhysBody*>* bumper = circles.getFirst();
	while (bumper != NULL)
	{
		if (bodyB == bumper->data)
		{
			b2Vec2 force(bodyA->body->GetWorldCenter() - bodyB->body->GetWorldCenter());
			force *= 3;
			bodyA->body->ApplyLinearImpulse(force, bodyA->body->GetWorldCenter(), true);
			App->audio->PlayFx(bonus_fx);
		}
		bumper = bumper->next;
	}

	if (bodyB == entryLevel && currentLvl == START)
	{
		currentLvl = FLOOR;
	}
	if (bodyB == entrySlide)
	{
		currentLvl = SLIDE;
		ballList.getLast()->data->GetPosition(x, y);
		ballList.getLast()->data->body->ApplyLinearImpulse(b2Vec2(0, -50), b2Vec2(x, y), true);
	}
	if (bodyB == exitSlide && currentLvl == SLIDE)
	{
		currentLvl = FLOOR;
	}

	if (bodyB == dieSensor)
	{
		dead = true;
		
	}
	if (bodyB == leftLifeSavour)
	{
		/*ballList.getLast()->data->GetPosition(x, y);
		ballList.getLast()->data->body->ApplyLinearImpulse(b2Vec2(0,-15),b2Vec2(x,y), true);*/
		b2Vec2 force(bodyA->body->GetWorldCenter() - bodyB->body->GetWorldCenter());
		force *= 12;
		bodyA->body->ApplyLinearImpulse(force, bodyA->body->GetWorldCenter(), true);
		App->audio->PlayFx(bonus_fx);
		lBlock = true;
	}
	if (bodyB == rightLifeSavour)
	{
		/*ballList.getLast()->data->GetPosition(x, y);
		ballList.getLast()->data->body->ApplyLinearImpulse(b2Vec2(0,-15),b2Vec2(x,y), true);*/
		b2Vec2 force(bodyA->body->GetWorldCenter() - bodyB->body->GetWorldCenter());
		force *= 12;
		bodyA->body->ApplyLinearImpulse(force, bodyA->body->GetWorldCenter(), true);
		App->audio->PlayFx(bonus_fx);
		rBlock = true;
	}
}

