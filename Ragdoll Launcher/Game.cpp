#include "Game.h"
#include "Box2DHelper.h"

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo) 
{
    wnd = new sf::RenderWindow(sf::VideoMode(ancho, alto), titulo);
    wnd->setVerticalSyncEnabled(true);
    fps = 60.f;
    frameTime = 1.6f / fps;
    wnd->setFramerateLimit(fps);
    renderer = new SFMLRenderer(wnd);

    InitPhysics();
    SetZoom();
}

// Método principal que maneja el bucle del juego
void Game::Run()
{
    while (wnd->isOpen())
    {
        DoEvents();
        UpdatePhysics();
        DrawGame();
    }
}

// Actualiza la simulación física
void Game::UpdatePhysics()
{ 
   for (auto& obstacle : obstacles) {
        if (obstacle.type == Obstacle::BARREL && obstacle.isStatic && obstacle.body->IsAwake()) {
            obstacle.body->SetGravityScale(1.0f);
            obstacle.isStatic = false;
            obstacle.body->SetType(b2_dynamicBody);
            obstacle.body->ResetMassData();
        }
    }
    phyWorld->Step(frameTime, 8, 8);
    phyWorld->ClearForces();
}

// Dibuja los elementos del juego en la ventana
void Game::DrawGame()
{
    wnd->clear(clearColor);
    phyWorld->DebugDraw();
    wnd->display();
}

void Game::DoEvents()
{
    sf::Event evt;
    while (wnd->pollEvent(evt)) {
        if (evt.type == sf::Event::Closed)
            wnd->close();

        if (evt.type == sf::Event::MouseButtonPressed &&
            evt.mouseButton.button == sf::Mouse::Left) {
            ShootRagdoll();
        }
        UpdateCannonAim();
    }
}

// Configura el área visible en la ventana de renderizado
void Game::SetZoom()
{
    View camara;
    camara.setSize(100.0f, 100.0f);
    camara.setCenter(50.0f, 50.0f);
    wnd->setView(camara);

}

void Game::CreateCannon() {
    const float cannonScale = 2.0f;
    const b2Vec2 cannonPos(5.0f, 90.0f); //Posición fija en esquina inferior izquierda

    //Base fija del cañón (estática)
    cannonBase = Box2DHelper::CreateRectangularStaticBody(phyWorld,
        1.5f * cannonScale, 1.5f * cannonScale);
    cannonBase->SetTransform(cannonPos, 0.0f);

    //Barril del cañón (cinemático - no afectado por gravedad)
    b2BodyDef barrelDef;
    barrelDef.type = b2_kinematicBody; // Cambio crucial
    barrelDef.position = cannonPos + b2Vec2(1.5f, 0.0f);
    cannonBarrel = phyWorld->CreateBody(&barrelDef);

    b2FixtureDef barrelFixture;
    b2PolygonShape barrelShape;
    barrelShape.SetAsBox(1.5f * cannonScale, 0.4f * cannonScale);
    barrelFixture.shape = &barrelShape;
    barrelFixture.density = 1.0f;
    cannonBarrel->CreateFixture(&barrelFixture);

    //Joint para rotación
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(cannonBase, cannonBarrel, cannonBase->GetWorldCenter());
    jointDef.enableLimit = true;
    jointDef.lowerAngle = -0.5f * b2_pi; //Limites de apuntado vertical
    jointDef.upperAngle = 0.8f * b2_pi;
    cannonJoint = (b2RevoluteJoint*)phyWorld->CreateJoint(&jointDef);
}

void Game::UpdateCannonAim() {

    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*wnd);
    sf::Vector2f mouseWorldPos = wnd->mapPixelToCoords(mousePixelPos);

    b2Vec2 cannonPos = cannonBarrel->GetPosition();
    b2Vec2 targetPos(mouseWorldPos.x, mouseWorldPos.y);

    //Calcular ángulo con límites mejorados
    b2Vec2 direction = targetPos - cannonPos;
    float angle = atan2f(direction.y, direction.x);

    //Límites más amplios y prevención de clipping
    float minAngle = -0.5f * b2_pi; //
    float maxAngle = 0.8f * b2_pi;  //

    //Prevenir que la punta toque el suelo
    b2Vec2 barrelTip = cannonBarrel->GetWorldPoint(b2Vec2(3.0f, 0.0f));
    if (barrelTip.y > 95.0f) {
        angle = std::max(minAngle, std::min(angle, -0.1f * b2_pi));
    }
    else {
        angle = std::max(minAngle, std::min(angle, maxAngle));
    }
    cannonBarrel->SetTransform(cannonPos, angle);
}

void Game::ShootRagdoll()
{
    b2Vec2 barrelTip = cannonBarrel->GetWorldPoint(b2Vec2(3.0f, 0.0f));

    //Obtener posición del mouse en coordenadas del mundo
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*wnd);
    sf::Vector2f mouseWorldPos = wnd->mapPixelToCoords(mousePixelPos);
    b2Vec2 targetPos(mouseWorldPos.x, mouseWorldPos.y);

    //Calcular distancia entre el cañón y el mouse
    float distance = b2Distance(barrelTip, targetPos);

    const float minPower = 15.0f;
    const float maxPower = 150.0f;
    const float maxDistance = 90.0f;

    //Calcular potencia proporcional
    float power = minPower + (std::min(distance, maxDistance) / maxDistance * (maxPower - minPower));

    //Dirección del disparo
    float angle = cannonBarrel->GetAngle();
    b2Vec2 direction(cosf(angle), sinf(angle));

    CreateRagdoll(barrelTip);

    //Configurar propiedades de colisión del ragdoll
    for (auto part : currentRagdollParts) {
        part->SetBullet(true);
        part->SetAwake(true);

        //Configurar datos de usuario para identificar como ragdoll
        part->GetUserData().pointer = 1;

        //Aumentar masa y aplicar fuerza
        b2Fixture* fixture = part->GetFixtureList();
        if (fixture) {
            fixture->SetDensity(0.3f);
            part->ResetMassData();
        }

        part->ApplyLinearImpulseToCenter(b2Vec2(direction.x * power, direction.y * power), true);
    }

    //Almacenar ragdoll
    Ragdoll newRagdoll;
    newRagdoll.parts = currentRagdollParts;
    newRagdoll.hasBeenFired = true;
    ragdolls.push_back(newRagdoll);

    currentRagdollParts.clear();
}

void Game::CreateRagdoll(const b2Vec2& position)
{
    currentRagdollParts.clear();
    const float scale = 1.5f;
    const float density = 0.5f;
    const float friction = 0.1f;
    const float restitution = 0.5f;

    // Torso
    b2Body* torso = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 1.5f * scale, 3.0f * scale, density, friction, restitution);
    torso->SetTransform(position, 0.0f);
    currentRagdollParts.push_back(torso);

    // Cabeza
    b2Body* head = Box2DHelper::CreateCircularDynamicBody(phyWorld, 0.8f * scale, density, friction, restitution);
    head->SetTransform(position + b2Vec2(0.0f, -2.2f * scale), 0.0f);
    currentRagdollParts.push_back(head);

    // Brazos
    b2Body* upperArmL = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 1.5f * scale, 0.6f * scale, density, friction, restitution);
    upperArmL->SetTransform(position + b2Vec2(-1.8f * scale, -1.0f * scale), -0.3f);
    currentRagdollParts.push_back(upperArmL);

    b2Body* upperArmR = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 1.5f * scale, 0.6f * scale, density, friction, restitution);
    upperArmR->SetTransform(position + b2Vec2(1.8f * scale, -1.0f * scale), 0.3f);
    currentRagdollParts.push_back(upperArmR);

    // Piernas
    b2Body* upperLegL = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 0.8f * scale, 2.0f * scale, density, friction, restitution);
    upperLegL->SetTransform(position + b2Vec2(-0.5f * scale, 2.5f * scale), -0.1f);
    currentRagdollParts.push_back(upperLegL);

    b2Body* upperLegR = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 0.8f * scale, 2.0f * scale, density, friction, restitution);
    upperLegR->SetTransform(position + b2Vec2(0.5f * scale, 2.5f * scale), 0.1f);
    currentRagdollParts.push_back(upperLegR);

    // Creación de joints
    b2RevoluteJointDef jointDef;
    jointDef.lowerAngle = -0.8f * b2_pi;
    jointDef.upperAngle = 0.8f * b2_pi;
    jointDef.enableLimit = true;
    jointDef.enableMotor = true;
    jointDef.maxMotorTorque = 5.0f;

    // Cabeza
    jointDef.Initialize(torso, head, torso->GetWorldPoint(b2Vec2(0, -1.5f * scale)));
    phyWorld->CreateJoint(&jointDef);

    // Brazos
    jointDef.Initialize(torso, upperArmL, torso->GetWorldPoint(b2Vec2(-0.9f * scale, -0.8f * scale)));
    phyWorld->CreateJoint(&jointDef);

    jointDef.Initialize(torso, upperArmR, torso->GetWorldPoint(b2Vec2(0.9f * scale, -0.8f * scale)));
    phyWorld->CreateJoint(&jointDef);

    // Piernas
    jointDef.Initialize(torso, upperLegL, torso->GetWorldPoint(b2Vec2(-0.3f * scale, 1.5f * scale)));
    phyWorld->CreateJoint(&jointDef);

    jointDef.Initialize(torso, upperLegR, torso->GetWorldPoint(b2Vec2(0.3f * scale, 1.5f * scale)));
    phyWorld->CreateJoint(&jointDef);

    for (auto part : currentRagdollParts) {
        part->SetLinearDamping(0.3f);
        part->SetAngularDamping(0.8f);
    }
}

b2Vec2 Game::GenerateRandomPosition() {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    //Área segura (evitar bordes)
    const float margin = 15.0f;
    float x = margin + static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / (100.0f - 2 * margin)));
    float y = margin + static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / (60.0f - 2 * margin)));

    //Lista de áreas prohibidas (x1, y1, x2, y2)
    const std::vector<std::tuple<float, float, float, float>> prohibidoAreas = {
        {70.0f, 74.0f, 40.0f, 76.0f},   //Plataforma inclinada
        {72.5f, 19.0f, 97.5f, 21.0f}     //Plataforma horizontal
    };

    // Verificar posición válida
    bool validPosition;
    do {
        validPosition = true;
        for (const auto& area : prohibidoAreas) {
            if (x > std::get<0>(area) && x < std::get<2>(area) &&
                y > std::get<1>(area) && y < std::get<3>(area)) {
                validPosition = false;
                // Regenerar posición
                x = margin + static_cast<float>(rand()) /
                    (static_cast<float>(RAND_MAX / (100.0f - 2 * margin)));
                y = margin + static_cast<float>(rand()) /
                    (static_cast<float>(RAND_MAX / (60.0f - 2 * margin)));
                break;
            }
        }
    } while (!validPosition);

    return b2Vec2(x, y);
}

// Inicializa el mundo físico y los elementos estáticos del juego
void Game::InitPhysics()
{
    phyWorld = new b2World(b2Vec2(0, 14.8f));
    phyWorld->SetDebugDraw(renderer);
    renderer->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
    phyWorld->SetContinuousPhysics(true);

    const float worldWidth = 100.0f;
    const float worldHeight = 100.0f;
    const float wallThickness = 2.0f;

    // Suelo (centrado en la parte inferior)
    groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, worldWidth, wallThickness);
    groundBody->SetTransform(b2Vec2(worldWidth / 2, worldHeight - wallThickness / 2), 0.0f);

    // Pared izquierda (centrada en el borde izquierdo)
    leftWall = Box2DHelper::CreateRectangularStaticBody(phyWorld, wallThickness, worldHeight);
    leftWall->SetTransform(b2Vec2(wallThickness / 2, worldHeight / 2), 0.0f);

    // Pared derecha (centrada en el borde derecho)
    rightWall = Box2DHelper::CreateRectangularStaticBody(phyWorld, wallThickness, worldHeight);
    rightWall->SetTransform(b2Vec2(worldWidth - wallThickness / 2, worldHeight / 2), 0.0f);

    // Techo (opcional, centrado en la parte superior)
    ceiling = Box2DHelper::CreateRectangularStaticBody(phyWorld, worldWidth, wallThickness);
    ceiling->SetTransform(b2Vec2(worldWidth / 2, wallThickness / 2), 0.0f);

    CreateCannon();

    // Obstáculos estáticos (plataformas)
    Obstacle platform1;
    platform1.body = Box2DHelper::CreateRectangularStaticBody(phyWorld, 25.0f, 1.0f);
    platform1.body->SetTransform(b2Vec2(85.0f, 20.0f), 0.0f);
    platform1.isStatic = true;
    platform1.type = Obstacle::PLATFORM;
    obstacles.push_back(platform1);

    Obstacle platform2;
    platform2.body = Box2DHelper::CreateRectangularStaticBody(phyWorld, 30.0f, 1.0f);
    platform2.body->SetTransform(b2Vec2(55.0f, 75.0f), 0.3f);
    platform2.isStatic = true;
    platform2.type = Obstacle::PLATFORM;
    obstacles.push_back(platform2);

    // Obstáculo estático (columna - color gris)
    Obstacle column;
    column.body = Box2DHelper::CreateRectangularStaticBody(phyWorld, 2.0f, 20.0f);
    column.body->SetTransform(b2Vec2(90.0f, 85.0f), 0.0f);
    column.isStatic = true;
    column.type = Obstacle::COLUMN;
    obstacles.push_back(column);

    // Crear 12 barriles en posiciones aleatorias
    const float barrelRadius = 2.0f;
    const float barrelDensity = 0.3f; 
    const float barrelFriction = 0.3f;
    const float barrelRestitution = 0.5f;

    for (int i = 0; i < 12; ++i) {
        Obstacle barrel;
        b2BodyDef barrelDef;
        barrelDef.type = b2_dynamicBody;
        barrelDef.position = GenerateRandomPosition();
        barrelDef.awake = false;
        barrel.body = phyWorld->CreateBody(&barrelDef);

        b2FixtureDef fixtureDef;
        b2CircleShape circleShape;
        circleShape.m_radius = barrelRadius;
        fixtureDef.shape = &circleShape;
        fixtureDef.density = barrelDensity;
        fixtureDef.friction = barrelFriction;
        fixtureDef.restitution = barrelRestitution;
        barrel.body->CreateFixture(&fixtureDef);

        //Configuración especial para barriles
        barrel.isStatic = true;  //Queremos que comiencen estáticos
        barrel.type = Obstacle::BARREL;

        //Forzar comportamiento estático inicial
        barrel.body->SetGravityScale(0.0f);
        obstacles.push_back(barrel);
    }
}
