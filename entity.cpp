// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 entity.cpp v1.4
// This version of entity.cpp is modified to record the ends of the
// collision vector so a line may be drawn.
// Last Modification Apr-3-2013

#include "entity.h"

//=============================================================================
// constructor
//=============================================================================
Entity::Entity() : Image()
{
    radius = 1.0;
    edge.left = -1;
    edge.top = -1;
    edge.right = 1;
    edge.bottom = 1;
    mass = 1.0;
    velocity.x = 0.0;
    velocity.y = 0.0;
    deltaV.x = 0.0;
    deltaV.y = 0.0;
    active = true;                  // the entity is active
    rotatedBoxReady = false;
    collisionType = entityNS::CIRCLE;
    health = 100;
    gravity = entityNS::GRAVITY;
    pixelsColliding = 0;
	position = VECTOR2(0,0);
}

//=============================================================================
// Initialize the Entity.
// Pre: *gamePtr = pointer to Game object
//      width = width of Image in pixels  (0 = use full texture width)
//      height = height of Image in pixels (0 = use full texture height)
//      ncols = number of columns in texture (1 to n) (0 same as 1)
//      *textureM = pointer to TextureManager object
// Post: returns true if successful, false if failed
//=============================================================================
bool Entity::initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM)
{
    input = gamePtr->getInput();            // the input system
    return(Image::initialize(gamePtr->getGraphics(), width, height, ncols, textureM));
}

//=============================================================================
// activate the entity
//=============================================================================
void Entity::activate()
{
    active = true;
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Entity::update(float frameTime)
{
    velocity += deltaV;
    deltaV.x = 0;
    deltaV.y = 0;
    Image::update(frameTime);
    rotatedBoxReady = false;    // for rotatedBox collision detection
}

//=============================================================================
// ai (artificial intelligence)
// typically called once per frame
// performs ai calculations, ent is passed for interaction
//=============================================================================
void Entity::ai(float frameTime, Entity &ent)
{}

//=============================================================================
// Perform collision detection between this entity and the other Entity.
// Each entity must use a single collision type. Complex shapes that require
// multiple collision types may be done by treating each part as a separate
// entity or by using PIXEL_PERFECT collision.
// Typically called once per frame.
// The collision types: CIRCLE, BOX, ROTATED_BOX or PIXEL_PERFECT.
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::collidesWith(Entity &ent, VECTOR2 &collisionVector)
{ 
    // if either entity is not active then no collision may occcur
    if (!active || !ent.getActive())    
        return false;

    // If both entities are using CIRCLE collision
    if (collisionType == entityNS::CIRCLE && ent.getCollisionType() == entityNS::CIRCLE)
        return collideCircle(ent, collisionVector);
    // If both entities are using BOX collision
    if (collisionType == entityNS::BOX && ent.getCollisionType() == entityNS::BOX)
        return collideBox(ent, collisionVector);
    // If either entity is using PIXEL_PERFECT collision
    if (collisionType == entityNS::PIXEL_PERFECT || ent.getCollisionType() == entityNS::PIXEL_PERFECT)
        return collidePixelPerfect(ent, collisionVector);
    // All other combinations use separating axis test
    // If neither entity uses CIRCLE collision
    if (collisionType != entityNS::CIRCLE && ent.getCollisionType() != entityNS::CIRCLE)
        return collideRotatedBox(ent, collisionVector);
    else    // one of the entities is a circle
        if (collisionType == entityNS::CIRCLE)  // if this entity uses CIRCLE collision
        {
            // Check for collision from other box with our circle
            bool collide = ent.collideRotatedBoxCircle(*this, collisionVector); 
            // Put the collision vector in the proper direction
            collisionVector *= -1;              // reverse collision vector
            return collide;
        }
        else    // the other entity uses CIRCLE collision
            return collideRotatedBoxCircle(ent, collisionVector);
    return false;
}

//=============================================================================
// Circular collision detection method
// Called by collision(), default collision detection method
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::collideCircle(Entity &ent, VECTOR2 &collisionVector)
{
    // difference between centers
    distSquared = *getCenter() - *ent.getCenter();
    distSquared.x = distSquared.x * distSquared.x;      // difference squared
    distSquared.y = distSquared.y * distSquared.y;

    // Calculate the sum of the radii (adjusted for scale)
    sumRadiiSquared = (radius*getScaleX()) + (ent.radius*ent.getScaleX());
    sumRadiiSquared *= sumRadiiSquared;                 // square it

    // if entities are colliding
    if(distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // set collision vector
        collisionVector = *ent.getCenter() - *getCenter();
        collisionCenter = *getCenter();
        ent.setCollisionCenter(*ent.getCenter());
        return true;
    }
    return false;   // not colliding
}

//=============================================================================
// Axis aligned bounding box collision detection method
// Called by collision()
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::collideBox(Entity &ent, VECTOR2 &collisionVector)
{
    // if either entity is not active then no collision may occcur
    if (!active || !ent.getActive())
        return false;

    // Check for collision using Axis Aligned Bounding Box.
    if( (getCenterX() + edge.right*getScaleX() >= ent.getCenterX() + ent.getEdge().left*ent.getScaleX()) && 
        (getCenterX() + edge.left*getScaleX() <= ent.getCenterX() + ent.getEdge().right*ent.getScaleX()) &&
        (getCenterY() + edge.bottom*getScaleY() >= ent.getCenterY() + ent.getEdge().top*ent.getScaleY()) && 
        (getCenterY() + edge.top*getScaleY() <= ent.getCenterY() + ent.getEdge().bottom*ent.getScaleY()) )
    {
        // set collision vector
        collisionVector = *ent.getCenter() - *getCenter();
        collisionCenter = *getCenter();
        ent.setCollisionCenter(*ent.getCenter());
        return true;
    }
    return false;
}

//=============================================================================
// Rotated Box collision detection method
// Called by collision()
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
// Uses Separating Axis Test to detect collision. 
// The separating axis test:
//   Two boxes are not colliding if their projections onto a line do not overlap.
//=============================================================================
bool Entity::collideRotatedBox(Entity &ent, VECTOR2 &collisionVector)
{
    computeRotatedBox();                    // prepare rotated box
    ent.computeRotatedBox();                // prepare rotated box
    if (projectionsOverlap(ent, collisionVector) && ent.projectionsOverlap(*this, collisionVector))
        return true;
    return false;
}

//=============================================================================
// Projects other box onto this edge01 and edge03.
// Called by collideRotatedBox()
// Post: returns true if projections overlap, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::projectionsOverlap(Entity &ent, VECTOR2 &collisionVector)
{
    float projection, min01, max01, min03, max03, minOverlap, minOverlap2;

    // project other box onto edge01
    projection = graphics->Vector2Dot(&edge01, ent.getCorner(0)); // project corner 0
    min01 = projection;
    max01 = projection;
    // for each remaining corner
    for(int c=1; c<4; c++)
    {
        // project corner onto edge01
        projection = graphics->Vector2Dot(&edge01, ent.getCorner(c));
        if (projection < min01)
            min01 = projection;
        else if (projection > max01)
            max01 = projection;
    }
    // if projections do not overlap
    if (min01 > edge01Max || max01 < edge01Min)
        return false;                       // no collision is possible
    // project other box onto edge03
    projection = graphics->Vector2Dot(&edge03, ent.getCorner(0)); // project corner 0
    min03 = projection;
    max03 = projection;
    // for each remaining corner
    for(int c=1; c<4; c++)
    {
        // project corner onto edge03
        projection = graphics->Vector2Dot(&edge03, ent.getCorner(c));
        if (projection < min03)
            min03 = projection;
        else if (projection > max03)
            max03 = projection;
    }
    if (min03 > edge03Max || max03 < edge03Min) // if projections do not overlap
        return false;                       // no collision is possible

    // The edge with the smallest overlapping section is the edge where
    // the collision is occuring. The collision vector is created perpendicular
    // to the collision edge.
    // min01, min03, max01, max03 are the projection limits for one object and
    // edge01Min, edge01Max, edge03Min, edge03Max are the projection limits
    // for the other object. This code was contributed to the programming2dgames
    // forum by user stbn. The technique is described at 
    // www.metanetsoftware.com/technique/tutorialA.html
    if (min01 < edge01Min)
    {
        minOverlap = max01 - edge01Min;
        collisionVector = corners[1] - corners[0];
        collisionCenter = corners[0];
        ent.setCollisionCenter(corners[1]);
    }
    else
    {
        minOverlap = edge01Max - min01;
        collisionVector = corners[0] - corners[1];
        collisionCenter = corners[1];
        ent.setCollisionCenter(corners[0]);
    }
    if (min03 < edge03Min)
    {
        minOverlap2 = max03 - edge03Min;
        if (minOverlap2 < minOverlap)
        {
            collisionVector = corners[3] - corners[0];
            collisionCenter = corners[0];
            ent.setCollisionCenter(corners[3]);
        }
    }
    else
    {
        minOverlap2 = edge03Max - min03;
        if (minOverlap2 < minOverlap)
        {
            collisionVector = corners[0] - corners[3];
            collisionCenter = corners[3];
            ent.setCollisionCenter(corners[0]);
        }
    }
    return true;                            // projections overlap
}

//=============================================================================
// Rotated Box and Circle collision detection method
// Called by collision()
// Uses separating axis test on edges of box and radius of circle.
// If the circle center is outside the lines extended from the collision box
// edges (also known as the Voronoi region) then the nearest box corner is checked
// for collision using a distance check.
// The nearest corner is determined from the overlap tests.
//
//   Voronoi0 |   | Voronoi1
//         ---0---1---
//            |   |
//         ---3---2---
//   Voronoi3 |   | Voronoi2
//
// Pre: This entity must be rotated box and other entity (ent) must be circle.
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::collideRotatedBoxCircle(Entity &ent, VECTOR2 &collisionVector)
{
    float min01, min03, max01, max03, center01, center03, minOverlap, minOverlap2;

    computeRotatedBox();                    // prepare rotated box

    // project circle center onto edge01
    center01 = graphics->Vector2Dot(&edge01, ent.getCenter());
    min01 = center01 - ent.getRadius()*ent.getScaleX(); // min and max are Radius from center
    max01 = center01 + ent.getRadius()*ent.getScaleX();
    if (min01 > edge01Max || max01 < edge01Min) // if projections do not overlap
        return false;                       // no collision is possible
        
    // project circle center onto edge03
    center03 = graphics->Vector2Dot(&edge03, ent.getCenter());
    min03 = center03 - ent.getRadius()*ent.getScaleX(); // min and max are Radius from center
    max03 = center03 + ent.getRadius()*ent.getScaleX();
    if (min03 > edge03Max || max03 < edge03Min) // if projections do not overlap
        return false;                       // no collision is possible

    // circle projection overlaps box projection
    // check to see if circle is in voronoi region of collision box
    if(center01 < edge01Min && center03 < edge03Min)    // if circle in Voronoi0
        return collideCornerCircle(corners[0], ent, collisionVector);
    if(center01 > edge01Max && center03 < edge03Min)    // if circle in Voronoi1
        return collideCornerCircle(corners[1], ent, collisionVector);
    if(center01 > edge01Max && center03 > edge03Max)    // if circle in Voronoi2
        return collideCornerCircle(corners[2], ent, collisionVector);
    if(center01 < edge01Min && center03 > edge03Max)    // if circle in Voronoi3
        return collideCornerCircle(corners[3], ent, collisionVector);

    // Circle not in voronoi region so it is colliding with edge of box
    // The edge with the smallest overlapping section is the edge where
    // the collision is occuring. The collision vector is created perpendicular
    // to the collision edge.
    // min01, min03, max01, max03 are the projection limits for one object and
    // edge01Min, edge01Max, edge03Min, edge03Max are the projection limits
    // for the other object. This code was contributed to the programming2dgames
    // forum by user stbn. The technique is described at 
    // www.metanetsoftware.com/technique/tutorialA.html
    if (min01 < edge01Min)
    {
        minOverlap = max01 - edge01Min;
        collisionVector = corners[1] - corners[0];
        collisionCenter = corners[0];
        ent.setCollisionCenter(corners[1]);
    }
    else
    {
        minOverlap = edge01Max - min01;
        collisionVector = corners[0] - corners[1];
        collisionCenter = corners[1];
        ent.setCollisionCenter(corners[0]);
    }
    if (min03 < edge03Min)
    {
        minOverlap2 = max03 - edge03Min;
        if (minOverlap2 < minOverlap)
        {
            collisionVector = corners[3] - corners[0];
            collisionCenter = corners[0];
            ent.setCollisionCenter(corners[3]);
        }
    }
    else
    {
        minOverlap2 = edge03Max - min03;
        if (minOverlap2 < minOverlap)
        {
            collisionVector = corners[0] - corners[3];
            collisionCenter = corners[3];
            ent.setCollisionCenter(corners[0]);
        }
    }
    return true;
}

//=============================================================================
// The box corner is checked for collision with circle using a distance check.
// Called by collideRotatedBoxCircle()
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::collideCornerCircle(VECTOR2 corner, Entity &ent, VECTOR2 &collisionVector)
{
    distSquared = corner - *ent.getCenter();            // corner - circle
    distSquared.x = distSquared.x * distSquared.x;      // difference squared
    distSquared.y = distSquared.y * distSquared.y;

    // Calculate the sum of the radii, then square it
    sumRadiiSquared = ent.getRadius()*ent.getScaleX();   // (0 + circleR)
    sumRadiiSquared *= sumRadiiSquared;                 // square it

    // if corner and circle are colliding
    if(distSquared.x + distSquared.y <= sumRadiiSquared)
    {
        // set collision vector
        collisionVector = *ent.getCenter() - corner;
        collisionCenter = corner;
        ent.setCollisionCenter(*ent.getCenter());
        return true;
    }
    return false;
}

//=============================================================================
// Compute corners of rotated box, projection edges and min and max projections
// 0---1  corner numbers
// |   |
// 3---2
//=============================================================================
void Entity::computeRotatedBox()
{
    if(rotatedBoxReady)
        return;
    float projection;

    VECTOR2 rotatedX(cos(spriteData.angle), sin(spriteData.angle));
    VECTOR2 rotatedY(-sin(spriteData.angle), cos(spriteData.angle));

    const VECTOR2 *center = getCenter();
    corners[0] = *center + rotatedX * ((float)edge.left*getScaleX())  +
                           rotatedY * ((float)edge.top*getScaleY());
    corners[1] = *center + rotatedX * ((float)edge.right*getScaleX()) + 
                           rotatedY * ((float)edge.top*getScaleY());
    corners[2] = *center + rotatedX * ((float)edge.right*getScaleX()) + 
                           rotatedY * ((float)edge.bottom*getScaleY());
    corners[3] = *center + rotatedX * ((float)edge.left*getScaleX())  +
                           rotatedY * ((float)edge.bottom*getScaleY());

    // corners[0] is used as origin
    // The two edges connected to corners[0] are used as the projection lines
    edge01 = VECTOR2(corners[1].x - corners[0].x, corners[1].y - corners[0].y);
    graphics->Vector2Normalize(&edge01);
    edge03 = VECTOR2(corners[3].x - corners[0].x, corners[3].y - corners[0].y);
    graphics->Vector2Normalize(&edge03);

    // this entities min and max projection onto edges
    projection = graphics->Vector2Dot(&edge01, &corners[0]);
    edge01Min = projection;
    edge01Max = projection;
    // project onto edge01
    projection = graphics->Vector2Dot(&edge01, &corners[1]);
    if (projection < edge01Min)
        edge01Min = projection;
    else if (projection > edge01Max)
        edge01Max = projection;
    // project onto edge03
    projection = graphics->Vector2Dot(&edge03, &corners[0]);
    edge03Min = projection;
    edge03Max = projection;
    projection = graphics->Vector2Dot(&edge03, &corners[3]);
    if (projection < edge03Min)
        edge03Min = projection;
    else if (projection > edge03Max)
        edge03Max = projection;

    rotatedBoxReady = true;
}

//=============================================================================
// Pixel Perfect collision detection method
// Called by collision()
// If the graphics card does not support a stencil buffer then CIRCLE
// collision is used.
// Post: returns true if collision, false otherwise
//       sets collisionVector if collision
//=============================================================================
bool Entity::collidePixelPerfect(Entity &ent, VECTOR2 &collisionVector)
{
    if(graphics->getStencilSupport() == false)  // if stencil not supported
        return (collideCircle(ent, collisionVector));   // use CIRCLE collision

    // get fresh texture because they may have been released
    ent.spriteData.texture = ent.textureManager->getTexture();
    spriteData.texture = textureManager->getTexture();

    // if pixels are colliding
    pixelsColliding = graphics->pixelCollision(ent.getSpriteData(), this->getSpriteData());
    if(pixelsColliding > 0)
    {
        // set collision vector to center of entity
        collisionVector = *ent.getCenter() - *getCenter();
        collisionCenter = *getCenter();
        ent.setCollisionCenter(*ent.getCenter());
        return true;
    }
    return false;
}

//=============================================================================
// Is this Entity outside the specified rectangle
// Post: returns true if outside rect, false otherwise
//=============================================================================
bool Entity::outsideRect(RECT rect)
{
    if( spriteData.x + spriteData.width*getScaleX() < rect.left || 
        spriteData.x > rect.right ||
        spriteData.y + spriteData.height*getScaleY() < rect.top || 
        spriteData.y > rect.bottom)
        return true;
    return false;
}

//=============================================================================
// damage
// This entity has been damaged by a weapon.
// Override this function in the inheriting class.
//=============================================================================
void Entity::damage(int weapon)
{}

//=============================================================================
// Entity bounces after collision with another entity
//=============================================================================
void Entity::bounce(VECTOR2 &collisionVector, Entity &ent)
{
    VECTOR2 Vdiff = ent.getVelocity() - velocity;
    VECTOR2 cUV = collisionVector;              // collision unit vector
    Graphics::Vector2Normalize(&cUV);
    float cUVdotVdiff = Graphics::Vector2Dot(&cUV, &Vdiff);
    float massRatio = 1.0f;
    if (getMass() != 0)
        massRatio *= (ent.getMass() / (getMass() + ent.getMass()));
    if(massRatio < 0.1f)
        massRatio = 0.1f;

    // Move entities out of collision along collision vector
    VECTOR2 cv;
    int count=10;   // loop limit
    do
    {
        setX(getX() - cUV.x);
        setY(getY() - cUV.y);
        rotatedBoxReady = false;
        count--;
    } while( this->collidesWith(ent, cv) && count);

    // bounce
    deltaV += ((massRatio * cUVdotVdiff) * cUV);
}

//=============================================================================
// Force of gravity on this entity from other entity
// Adds the gravitational force to the velocity vector of this entity
// force = GRAVITY * m1 * m2 / r*r
//                    2              2
//  r*r  =   (Ax - Bx)   +  (Ay - By)
//=============================================================================
void Entity::gravityForce(Entity *ent, float frameTime)
{
    // if either entity is not active then no gravity effect
    if (!active || !ent->getActive())
        return ;

    rr = pow((ent->getCenterX() - getCenterX()),2) + 
            pow((ent->getCenterY() - getCenterY()),2);
    force = gravity * ent->getMass() * mass/rr;

    // --- Using vector math to create gravity vector ---
    // Create vector between entities
    VECTOR2 gravityV(ent->getCenterX() - getCenterX(),
                        ent->getCenterY() - getCenterY());
    // Normalize the vector
    Graphics::Vector2Normalize(&gravityV);
    // Multipy by force of gravity to create gravity vector
    gravityV *= force * frameTime;
    // Add gravity vector to moving velocity vector to change direction
    velocity += gravityV;
}
