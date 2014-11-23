// Programming 2D Games
// Copyright (c) 2012 by: 
// Charles Kelly
// CollisionTypes ship.cpp v1.0

#include "ship.h"

//=============================================================================
// default constructor
//=============================================================================
Ship::Ship() : Entity()
{
    spriteData.width = shipNS::WIDTH;           // size of Ship1
    spriteData.height = shipNS::HEIGHT;
    spriteData.x = shipNS::X;                   // location on screen
    spriteData.y = shipNS::Y;
    spriteData.rect.bottom = shipNS::HEIGHT;    // rectangle to select parts of an image
    spriteData.rect.right = shipNS::WIDTH;
    velocity.x = 0;                             // velocity X
    velocity.y = 0;                             // velocity Y
    startFrame = 0;              // first frame of ship animation
    endFrame     = 0;              // last frame of ship animation
    currentFrame = startFrame;
    radius = shipNS::WIDTH/2.0;                 // collision radius
    collision = false;
    collisionType = entityNS::CIRCLE;
    target = false;
}

//=============================================================================
// Initialize the Ship.
// Post: returns true if successful, false if failed
//=============================================================================
bool Ship::initialize(Game *gamePtr, int width, int height, int ncols,
    TextureManager *textureM)
{
    return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Ship::update(float frameTime)
{
    switch (direction)                          // rotate ship
    {
    case shipNS::LEFT:
        spriteData.angle -= frameTime * shipNS::ROTATION_RATE;  // rotate left
        break;
    case shipNS::RIGHT:
        spriteData.angle += frameTime * shipNS::ROTATION_RATE;  // rotate right
        break;
    }
    spriteData.x += velocity.x * frameTime;
    velocity.x = 0;
    spriteData.y += velocity.y * frameTime;
    velocity.y = 0;

    Entity::update(frameTime);

    // wrap around screen
    if (spriteData.x > GAME_WIDTH)                  // if off screen right
        spriteData.x = -shipNS::WIDTH;              // position off screen left
    else if (spriteData.x < -shipNS::WIDTH)         // else if off screen left
        spriteData.x = GAME_WIDTH;                  // position off screen right
    if (spriteData.y < -shipNS::HEIGHT)             // if off screen top
        spriteData.y = GAME_HEIGHT;                 // position off screen bottom
    else if (spriteData.y > GAME_HEIGHT)            // else if off screen bottom
        spriteData.y = -shipNS::HEIGHT;             // position off screen top
}

