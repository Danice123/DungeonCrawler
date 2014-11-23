// Programming 2D Games
// Copyright (c) 2012 by: 
// Charles Kelly
// Collision Types ship.h v1.0

#ifndef _SHIP_H                 // Prevent multiple definitions if this 
#define _SHIP_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

class Ship;

#include "entity.h"
#include "constants.h"

namespace shipNS
{
    const int WIDTH = 64;                   // image width
    const int HEIGHT = 64;                  // image height
    const int X = GAME_WIDTH/2 - WIDTH/2;   // location on screen
    const int Y = GAME_HEIGHT/2 - HEIGHT/2;
    const float ROTATION_RATE = (float)PI/3; // radians per second
    const float SPEED = 100;                // 100 pixels per second
    enum DIRECTION {NONE, LEFT, RIGHT};     // rotation direction
}

// inherits from Entity class
class Ship : public Entity
{
private:
    shipNS::DIRECTION direction;    // direction of rotation
    bool collision;                 // true when ship is colliding
    bool target;                    // true if target, false if ship

public:
    // constructor
    Ship();

    // inherited member functions
    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM);
    void update(float frameTime);

    // Set collision Boolean
    void setCollision(bool c)
    {collision = c;}

    // Set collision type (NONE, CIRCLE, BOX, ROTATED_BOX)
    virtual void setCollisionType(entityNS::COLLISION_TYPE ctype)
    {collisionType = ctype;}

    // Set RECT structure used for BOX and ROTATED_BOX collision detection.
    void setEdge(RECT e) {edge = e;}

    // Set target
    void setTarget(bool t) {target = t;}

    // Get collision
    bool getCollision() {return collision;}

    // Get collision type
    entityNS::COLLISION_TYPE getCollisionType() {return collisionType;}

    // direction of rotation force
    void rotate(shipNS::DIRECTION dir) {direction = dir;}

    // move forward
    void forward()
    {
        velocity.x = (float)cos(spriteData.angle) * shipNS::SPEED;
        velocity.y = (float)sin(spriteData.angle) * shipNS::SPEED;
    }

    // move in reverse
    void reverse()
    {
        velocity.x = -(float)cos(spriteData.angle) * shipNS::SPEED;
        velocity.y = -(float)sin(spriteData.angle) * shipNS::SPEED;
    }
};
#endif

