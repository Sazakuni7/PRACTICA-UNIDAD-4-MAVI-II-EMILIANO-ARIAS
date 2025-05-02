#include <Box2D/Box2D.h>
#pragma once

class Box2DHelper
{
public:
	//-------------------------------------------------------------
	// Crea un body dinamico sin fixtures centrado en 0,0
	//-------------------------------------------------------------
	static b2Body* CreateDynamicBody(b2World* phyWorld)
	{
		// Declaramos el body
		b2Body* body;

		// Llenamos la definicion
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0.0f, 0.0f);
		body = phyWorld->CreateBody(&bodyDef);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body estatico sin fixtures centrado en 0,0
	//-------------------------------------------------------------
	static b2Body* CreateStaticBody(b2World* phyWorld)
	{
		// Declaramos el body
		b2Body* body;

		// Llenamos la definicion
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(0.0f, 0.0f);
		body = phyWorld->CreateBody(&bodyDef);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body cinematico sin fixtures centrado en 0,0
	//-------------------------------------------------------------
	static b2Body* CreateKinematicBody(b2World* phyWorld)
	{
		// Declaramos el body
		b2Body* body;

		// Llenamos la definicion
		b2BodyDef bodyDef;
		bodyDef.type = b2_kinematicBody;
		bodyDef.position.Set(0.0f, 0.0f);
		body = phyWorld->CreateBody(&bodyDef);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un fixture rectangular con el 0,0 en el medio
	//-------------------------------------------------------------
	static b2FixtureDef CreateRectangularFixtureDef(float sizeX, float sizeY, float density, float friction, float restitution)
	{
		b2PolygonShape* box = new b2PolygonShape();

		box->SetAsBox(sizeX / 2.0f, sizeY / 2.0f, b2Vec2(0.0f, 0.0f), 0.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = box;
		fixtureDef.density = density;
		fixtureDef.friction = friction;
		fixtureDef.restitution = restitution;

		return fixtureDef;
	}

	//-------------------------------------------------------------
	// Crea un fixture circular con el 0,0 en el medio
	//-------------------------------------------------------------
	static b2FixtureDef CreateCircularFixtureDef(float radius, float density, float friction, float restitution)
	{
		b2CircleShape* circle = new b2CircleShape();
		circle->m_p = b2Vec2(0.0f, 0.0f);
		circle->m_radius = radius;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = circle;
		fixtureDef.density = density;
		fixtureDef.friction = friction;
		fixtureDef.restitution = restitution;
		return fixtureDef;
	}

	//-------------------------------------------------------------
	// Crea un fixture poligonal dado un conj de puntos
	//-------------------------------------------------------------
	static b2FixtureDef CreatePolyFixtureDef(b2Vec2* v, int n, float density, float friction, float restitution)
	{
		b2PolygonShape* poly = new b2PolygonShape();
		poly->Set(v, n);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = poly;
		fixtureDef.density = density;
		fixtureDef.friction = friction;
		fixtureDef.restitution = restitution;
		return fixtureDef;
	}

	//-------------------------------------------------------------
	// Crea un fixture triangular (triangulo equilatero)
	//-------------------------------------------------------------
	static b2FixtureDef CreateTriangularFixtureDef(b2Vec2 c, float h, float density, float friction, float restitution)
	{
		b2Vec2 verts[3];
		verts[0].x = -h / 2;
		verts[0].y = -h / 2;
		verts[1].x = h / 2;
		verts[1].y = -h / 2;
		verts[2].x = 0;
		verts[2].y = h / 2;
		return CreatePolyFixtureDef(verts, 3, density, friction, restitution);
	}

	//-------------------------------------------------------------
	//Crea un body dinamico con un fixture rectangular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateRectangularDynamicBody(b2World* phyWorld, float sizeX, float sizeY, float density, float friction, float restitution)
	{
		b2Body* body = CreateDynamicBody(phyWorld);
		b2FixtureDef box = CreateRectangularFixtureDef(sizeX, sizeY, density, friction, restitution);
		body->CreateFixture(&box);
		return body;
	}

	//-------------------------------------------------------------
	//Crea un body cinematico con un fixture rectangular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateRectangularKinematicBody(b2World* phyWorld, float sizeX, float sizeY)
	{
		b2Body* body = CreateKinematicBody(phyWorld);
		b2FixtureDef box = CreateRectangularFixtureDef(sizeX, sizeY, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&box);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body estatico con un fixture rectangular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateRectangularStaticBody(b2World* phyWorld, float sizeX, float sizeY)
	{
		b2Body* body = CreateStaticBody(phyWorld);
		b2FixtureDef box = CreateRectangularFixtureDef(sizeX, sizeY, 0.0f, 0.4f, 0.0f);
		body->CreateFixture(&box);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body dinamico con un fixture circular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateCircularDynamicBody(b2World* phyWorld, float radius, float density, float friction, float restitution)
	{
		b2Body* body = CreateDynamicBody(phyWorld);
		b2FixtureDef cir = CreateCircularFixtureDef(radius, density, friction, restitution);
		body->CreateFixture(&cir);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body cinematico con un fixture circular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateCircularKinematicBody(b2World* phyWorld, float radius)
	{
		b2Body* body = CreateKinematicBody(phyWorld);
		b2FixtureDef cir = CreateCircularFixtureDef(radius, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&cir);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body estatico con un fixture circular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateCircularStaticBody(b2World* phyWorld, float radius)
	{
		b2Body* body = CreateStaticBody(phyWorld);
		b2FixtureDef cir = CreateCircularFixtureDef(radius, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&cir);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body dinamico con un fixture triangular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateTriangularDynamicBody(b2World* phyWorld, b2Vec2 center, float h, float density, float friction, float restitution)
	{
		b2Body* body = CreateDynamicBody(phyWorld);
		b2FixtureDef tri = CreateTriangularFixtureDef(center, h, density, friction, restitution);
		body->CreateFixture(&tri);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body cinematico con un fixture triangular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateTriangularKinematicBody(b2World* phyWorld, b2Vec2 center, float h)
	{
		b2Body* body = CreateKinematicBody(phyWorld);
		b2FixtureDef tri = CreateTriangularFixtureDef(center, h, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&tri);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body estatico con un fixture triangular centrado en el origen
	//-------------------------------------------------------------
	static b2Body* CreateTriangularStaticBody(b2World* phyWorld, b2Vec2 center, float h)
	{
		b2Body* body = CreateStaticBody(phyWorld);
		b2FixtureDef tri = CreateTriangularFixtureDef(center, h, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&tri);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body dinamico con un fixture poligonal
	//-------------------------------------------------------------
	static b2Body* CreatePolyDynamicBody(b2World* phyWorld, b2Vec2* v, int n, float density, float friction, float restitution)
	{
		b2Body* body = CreateDynamicBody(phyWorld);
		b2FixtureDef tri = CreatePolyFixtureDef(v, n, density, friction, restitution);
		body->CreateFixture(&tri);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body cinematico con un fixture poligonal
	//-------------------------------------------------------------
	static b2Body* CreatePolyKinematicBody(b2World* phyWorld, b2Vec2* v, int n)
	{
		b2Body* body = CreateKinematicBody(phyWorld);
		b2FixtureDef tri = CreatePolyFixtureDef(v, n, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&tri);
		return body;
	}

	//-------------------------------------------------------------
	// Crea un body estatico con un fixture poligonal
	//-------------------------------------------------------------
	static b2Body* CreatePolyStaticBody(b2World* phyWorld, b2Vec2* v, int n)
	{
		b2Body* body = CreateDynamicBody(phyWorld);
		b2FixtureDef tri = CreatePolyFixtureDef(v, n, 0.0f, 0.0f, 0.0f);
		body->CreateFixture(&tri);
		return body;
	}

	//-------------------------------------------------------------
	// Un Distance Joint entre dos cuerpos
	//-------------------------------------------------------------
	static b2DistanceJoint* CreateDistanceJoint(b2World* phyWorld, b2Body* first, b2Vec2 anchorFirst, b2Body* second, b2Vec2 anchorSecond,
		float distance, float frec, float dampRatio)
	{
		b2DistanceJointDef jointDef;
		jointDef.Initialize(first, second, anchorFirst, anchorSecond);
		jointDef.collideConnected = true;
		jointDef.length = distance;
		b2LinearStiffness(jointDef.stiffness, jointDef.damping, frec, dampRatio, jointDef.bodyA, jointDef.bodyB);
		b2DistanceJoint* distJoint = (b2DistanceJoint*)phyWorld->CreateJoint(&jointDef);
		return distJoint;
	}

	//-------------------------------------------------------------
	// Un Revolute Joint entre dos cuerpos
	//-------------------------------------------------------------
	static b2RevoluteJoint* CreateRevoluteJoint(b2World* phyWorld, b2Body* base, b2Vec2 anchorBase, b2Body* second,
		float angMin, float angMax, float motorSpeed, float maxTorque,
		bool enableMotor, bool enableLimits)
	{
		b2RevoluteJointDef jointDef;
		jointDef.Initialize(base, second, anchorBase);
		jointDef.lowerAngle = angMin;
		jointDef.upperAngle = angMax;
		jointDef.enableLimit = enableLimits;
		jointDef.maxMotorTorque = maxTorque;
		jointDef.motorSpeed = motorSpeed;
		jointDef.enableMotor = enableMotor;

		b2RevoluteJoint* revJoint = (b2RevoluteJoint*)phyWorld->CreateJoint(&jointDef);
		return revJoint;
	}

	//-------------------------------------------------------------
	// Un Prismatic Joint entre dos cuerpos
	//-------------------------------------------------------------
	static b2PrismaticJoint* CreatePrismaticJoint(b2World* phyWorld, b2Body* base, b2Vec2 anchorBase, b2Body* second,
		b2Vec2 axis, float distMin, float distMax, float motorSpeed, float maxForce,
		bool enableMotor, bool enableLimits)
	{
		b2PrismaticJointDef jointDef;
		jointDef.Initialize(base, second, anchorBase, axis);
		jointDef.lowerTranslation = distMin;
		jointDef.upperTranslation = distMax;
		jointDef.enableLimit = enableLimits;
		jointDef.maxMotorForce = maxForce;
		jointDef.motorSpeed = motorSpeed;
		jointDef.enableMotor = enableMotor;

		b2PrismaticJoint* prisJoint = (b2PrismaticJoint*)phyWorld->CreateJoint(&jointDef);
		return prisJoint;
	}

	//-------------------------------------------------------------
	// Una Pulley Joint entre dos cuerpos
	//-------------------------------------------------------------
	static b2PulleyJoint* CreatePulleyJoint(b2World* phyWorld, b2Body* first, b2Vec2 anchorFirst, b2Vec2 worldAnchor1,
		b2Body* second, b2Vec2 anchorSecond, b2Vec2 worldAnchor2, float ratio)
	{
		b2PulleyJointDef jointDef;
		jointDef.Initialize(first, second, worldAnchor1, worldAnchor2, anchorFirst, anchorSecond, ratio);

		b2PulleyJoint* pullJoint = (b2PulleyJoint*)phyWorld->CreateJoint(&jointDef);
		return pullJoint;
	}

	//-------------------------------------------------------------
	// Una Pulley Joint entre dos cuerpos
	//-------------------------------------------------------------
	static b2GearJoint* CreateGearJoint(b2World* phyWorld, b2Body* first, b2Body* second,
		b2Joint* Joint1, b2Joint* Joint2, float ratio)
	{
		b2GearJointDef jointDef;
		jointDef.bodyA = first;
		jointDef.bodyB = second;
		jointDef.joint1 = Joint1;
		jointDef.joint2 = Joint2;
		jointDef.ratio = ratio;

		b2GearJoint* gearJoint = (b2GearJoint*)phyWorld->CreateJoint(&jointDef);
		return gearJoint;
	}

	//-------------------------------------------------------------
	// Un Mouse Joint entre dos cuerpos
	//-------------------------------------------------------------
	static b2MouseJoint* CreateMouseJoint(b2World* phyWorld, b2Body* second, float maxForce)
	{
		b2MouseJointDef jointDef;

		jointDef.bodyA = CreateStaticBody(phyWorld);
		jointDef.bodyB = second;
		jointDef.target = second->GetWorldCenter();
		jointDef.maxForce = maxForce;

		b2MouseJoint* mouseJoint = (b2MouseJoint*)phyWorld->CreateJoint(&jointDef);
		return mouseJoint;
	}
};