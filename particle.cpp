#include "particle.h"

Particle::Particle()
{
	visible = false;
	loop = false;
}

void Particle::resetParticle()
{
	timeAlive = 0;
	visible = false;
	active = false;
	maxTimeAlive = 0;
	setX(0);
	setY(0);
}
bool Particle::initialize(Graphics *g, int width, int height, int ncols,
						  TextureManager *textureM)
{
	try{
		graphics = g;                               // the graphics object
		textureManager = textureM;                  // pointer to texture object

		spriteData.texture = textureManager->getTexture();
		if(width == 0)
			width = textureManager->getWidth();     // use full width of texture
		spriteData.width = width;
		if(height == 0)
			height = textureManager->getHeight();   // use full height of texture
		spriteData.height = height;
		cols = ncols;
		if (cols == 0)
			cols = 1;                               // if 0 cols use 1

		// configure spriteData.rect to draw currentFrame
		spriteData.rect.left = (currentFrame % cols) * spriteData.width;
		// right edge + 1
		spriteData.rect.right = spriteData.rect.left + spriteData.width;
		spriteData.rect.top = (currentFrame / cols) * spriteData.height;
		// bottom edge + 1
		spriteData.rect.bottom = spriteData.rect.top + spriteData.height;
	}
	catch(...) {return false;}
	initialized = true;                                // successfully initialized
	return true;
}

void Particle::update(float frametime)
{
	if (!active)
		return;
	timeAlive += frametime;
	if (timeAlive >= maxTimeAlive)
	{
		resetParticle();
		return;
	}
	// update physics and drawing stuff

	setX(getX() + velocity.x * frametime);
	setY(getY() + velocity.y * frametime);
	rotationValue += frametime;
	if (rotationValue> 2*2.14159) //prevent overrotation
		rotationValue = 0;
	setRadians(rotationValue);


}