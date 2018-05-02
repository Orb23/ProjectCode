#include "MeleeRuler.h"
#include "Constants.h"
#include "sfml_game\CollidingSpriteEntity.h"
#include "WitchBlastGame.h"
#include "DungeonMap.h"
#include "sfml_game\ImageManager.h"
#include "sfml_game\SoundManager.h"


MeleeRuler::MeleeRuler(float x, float y, float RulerLifeTime, enumMeleeType Meleetype, int level,int direction)
	: CollidingSpriteEntity(ImageManager::getInstance().getImage(IMAGE_RULER),x,y,RULER_WIDTH,RULER_HEIGHT)
{
	lifetime = RulerLifeTime;
	setDamages(INITIAL_RULER_DAMAGES);
	type = ENTITY_BOLT;
	this->level = level;
	// gets the frame image at v 0;
	frame = 0;
	setMap(game().getCurrentMap(), TILE_WIDTH, TILE_HEIGHT, 0, 0);
	this->Meleetype = Meleetype;
	enemyType = EnemyTypeNone;
	goThrough = false;
	hitNumber = 0;
	fromPlayer = true;

	///	Test if the ruler is upgraded or not  (WILL NOT BE IMPLEMENTED)
	//switch (Meleetype)
	//{
	//case ShotTypeDeterministic:
	//case ShotTypeStandard:  frame = 0; break;
	//case ShotTypeCold:
	//case ShotTypeIce:       frame = 2; break;
	//case ShotTypeStone:     frame = 4; break;
	//case ShotTypeLightning: frame = 5; goThrough = true; break;
	//case ShotTypeIllusion:  frame = 3; break;
	//case ShotTypeFire:      frame = 6; break;
	//case ShotTypePoison:    frame = 7; break;
	//case ShotTypeBomb:      frame = 8; damages = 0; sprite.setScale(1.0f, 1.0f); break;
	//}


	testWallsCollision = false;
	flying = false;
	critical = false;
	this->direction = direction;

	// avoid starting in wall
	if (y > ((MAP_HEIGHT - 1) * TILE_HEIGHT - 16))
		this->y = (MAP_HEIGHT - 1) * TILE_HEIGHT - 16;
}

int MeleeRuler::getDamages()
{
	return damages;
}

void MeleeRuler::setFromPlayer(bool fromPlayer)
{
	this->fromPlayer = fromPlayer;
}

bool MeleeRuler::isFromPlayer()
{
	return fromPlayer;
}


unsigned int MeleeRuler::getLevel()
{
	return level;
}

void MeleeRuler::setDamages(int damages)
{

	// Size of sprite grows relative to damage value
	this->damages = damages;

	//if (damages <= 4)  renderScale = 0.7f;
	//else if (damages <= 6)  renderScale = 0.78f;
	//else if (damages <= 8)  renderScale = 0.85f;
	//else if (damages <= 12) renderScale = 0.9f;
	//else if (damages <= 16) renderScale = 1.0f;
	//else if (damages <= 20) renderScale = 1.1f;
	//else if (damages <= 24) renderScale = 1.2f;
	//else if (damages <= 30) renderScale = 1.3f;
	//else renderScale = 1.4f;

	//sprite.scale(renderScale, renderScale);
}


void MeleeRuler::loseDamages(int damages)
{
	if (this->damages > damages) setDamages(this->damages - damages);
	else setDamages(0);
	critical = false;
}

enumMeleeType MeleeRuler::getMeleeType()
{
	return Meleetype;
}

enemyTypeEnum MeleeRuler::getEnemyType()
{
	return enemyType;
}

bool MeleeRuler::isCritical()
{
	return critical;
}


void MeleeRuler::setCritical(bool Critical)
{
	this->critical = Critical;
}

void MeleeRuler::setGoThrough(bool goThrough)
{
	this->goThrough = goThrough;
}

bool MeleeRuler::getGoThrough()
{
	return goThrough;
}

void MeleeRuler::onDying()
{
	isDying = true;
}

void MeleeRuler::stuck()
{
	SoundManager::getInstance().playSound(SOUND_WALL_IMPACT);

	onDying();
}


 //work needed
void MeleeRuler::animate(float delay)
{

	particleGenerator.GenerateParticles(frame, IMAGE_RULER, x, y, RULER_WIDTH, RULER_HEIGHT, Vector2D(0.0f, 0.0f),0, renderScale);

	z = y + height;

	//testWallsCollision = true;

	if (isCollidingWithMap())
	{
		//isDying = true;
		SoundManager::getInstance().playSound(SOUND_WALL_IMPACT);
		//for (int i = 0; i<5; i++) generateParticule(100.0f + rand() % 150);
	}
	else
	{
		float oldX = x;
		float oldY = y;
		CollidingSpriteEntity::animate(delay);

		if (game().getPlayer()->isEquiped(EQUIP_RAPID_SHOT) || damages < 5)
			particleGenerator.GenerateParticles(frame, IMAGE_RULER, (x + oldX) * 0.5f, (y + oldY) * 0.5f, RULER_WIDTH, RULER_HEIGHT, Vector2D(0.0f, 0.0f), 0, renderScale);
	}


	// key room collision
	if (game().getCurrentMap()->getRoomType() == roomTypeKey && !game().getCurrentMap()->isCleared())
	{
		sf::IntRect col1;
		col1.width = 198;
		col1.height = 68;
		col1.top = 254;
		col1.left = 380;

		sf::IntRect col2;
		col2.width = 68;
		col2.height = 198;
		col2.top = 189;
		col2.left = 445;

		//checks if Delta box has a delta point inside itself.
		if (boundingBox.intersects(col1) || boundingBox.intersects(col2))
		{
			game().activateKeyRoomEffect(false);
			if (x < 390) collideMapRight();
			else if (x > 565) collideMapLeft();
			else if (y < 265) collideMapBottom();
			else collideMapTop();
		}
	}

	testWallsCollision = false;
	calculateBB();

	if ((lifetime - age) < 0.2f)
	{
		if (age >= lifetime)
			sprite.setColor(sf::Color(255, 255, 255, 0));
		else
			sprite.setColor(sf::Color(255, 255, 255, (sf::Uint8)((lifetime - age) / 0.2f * 255)));
	}

	// Calculate velocity speed, Need to change this to something else to destroy the ruler
	//if (((velocity.x)*(velocity.x) + (velocity.y)*(velocity.y)) < 1500.0f) isDying = true;
	isDying = true;
}

// Displays the ruler hitbox
void MeleeRuler::render(sf::RenderTarget * app)
{
	
	CollidingSpriteEntity::render(app);
	if (game().getShowLogical())
	{
		displayBoundingBox(app);
	}
}

//calculate collision against walls
// Parameter in direction ?????????? pass in somehow by reference possibly.
void MeleeRuler::calculateBB()
{

	//original line
	//int colSize = testWallsCollision ? 1 : 10;

	// Code below is used to change the hitbox of the bolt.
	// if changes occur, it will be a set hitbox. (meaning the size is permanent
	//		no matter where you shoot).
	// Needs a separate function to detect of the hitbox is being created when the player
	//	is looking down or up to maintain its logical hitbox.

	//If else statement is needed to check if player is facing right or left.

	//sets the hitbox of the ruler for north and south position.
	if (direction == 8 || direction == 2) 
	{
		 colSize = 30;
		 rowSize = 10;
	}
	//sets the hitbox of the ruler for the east and west position.
	else if (direction == 4 || direction == 6)
	{
		 colSize = 10;
		 rowSize = 30;
	}

	boundingBox.left = x - rowSize;
	boundingBox.width = rowSize * 2;

	//boundingBox.left = x - colSize;
	//boundingBox.width = colSize * 2;
	boundingBox.top = y - colSize;
	boundingBox.height = colSize * 2;
}


//work needed
void MeleeRuler::collide()
{
	hitNumber++;
	if (fromPlayer)
	{
		if (goThrough)
		{
			if (damages > 0) return;
		}
	}

	isDying = true;

	//Again with velocity, need to change this. possibly
	//for (int i = 0; i<5; i++)
	//{
	//	Vector2D vel(40.0f + rand() % 50);
	//}

}

//checks if the wall is destructable. NEED TO FIND THIS WALL IN GAME TO TEST
bool MeleeRuler::collideWithMap(int direction)
{
	calculateBB();

	int xTile0 = (boundingBox.left - offsetX) / tileWidth;
	int xTilef = (boundingBox.left + boundingBox.width - offsetX) / tileWidth;
	int yTile0 = (boundingBox.top - offsetY) / tileHeight;
	int yTilef = (boundingBox.top + boundingBox.height - offsetY) / tileHeight;

	if (boundingBox.top < 0) yTile0 = -1;

	for (int xTile = xTile0; xTile <= xTilef; xTile++)
		for (int yTile = yTile0; yTile <= yTilef; yTile++)
		{
			if (dynamic_cast<DungeonMap*>(map)->isShootable(xTile, yTile) == false) return true;
		}

	return false;
}


// ALL collideMap___ needs to be reworked to not use velocity. (Melee stays put).
void MeleeRuler::collideMapRight()
{

	velocity.x = 0.0f;
	isDying = true;

	SoundManager::getInstance().playSound(SOUND_WALL_IMPACT);
	
	////Need to be reworked to not use velocity. (Melee stays put.)
	//for (int i = 0; i<5; i++)
	//{
	//	Vector2D vel(100.0f + rand() % 150);
	//}
}

void MeleeRuler::collideMapLeft()
{

	velocity.x = 0.0f;
	isDying = true;

	SoundManager::getInstance().playSound(SOUND_WALL_IMPACT);

	//Need to be reworked to not use velocity. (Melee stays put.)
	//for (int i = 0; i<5; i++)
	//{
	//	Vector2D vel(100.0f + rand() % 150);
	//}
}

void MeleeRuler::collideMapTop()
{

	velocity.y = 0.0f;
	isDying = true;

	SoundManager::getInstance().playSound(SOUND_WALL_IMPACT);
	//for (int i = 0; i<5; i++)
	//{
	//	Vector2D vel(100.0f + rand() % 150);
	//}
}

void MeleeRuler::collideMapBottom()
{
	velocity.y = 0.0f;
	isDying = true;

	SoundManager::getInstance().playSound(SOUND_WALL_IMPACT);
	//for (int i = 0; i<5; i++)
	//{
	//	Vector2D vel(100.0f + rand() % 150);
	//}
}





MeleeRuler::~MeleeRuler()
{
}

