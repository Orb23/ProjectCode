#pragma once
#include "Constants.h"
#include "sfml_game\CollidingSpriteEntity.h"
#include "BaseCreatureEntity.h"
#include "ParticleGenerator.h"

/*! \class MeleeRuler
* \brief stab by the player
*
*  MeleeRuler is a melee weapon used by the player.
*  It can collide with an enemy (to hurt him) or with the walls.
*/

class MeleeRuler : public CollidingSpriteEntity
{
public:


	MeleeRuler(float x, float y, float RulerLifeTime, enumShotType MeleeHit, int level);
	virtual void animate(float delay);
	virtual void render(sf::RenderTarget* app);
	void collide();

	int getDamages();
	unsigned int getLevel();
	void setDamages(int damages);
	void loseDamages(int damages);
	enumShotType getMeleeHit();
	bool isCritical();
	void setCritical(bool setCritical);

	// IMPORTANT WHENEVER IT INTERACTS WITH A WALL !!!!!!!!!!!!!!!!
	void setGoThrough(bool goThrough);
	bool getGoThrough();

	enemyTypeEnum getEnemyType();
	void setFromPlayer(bool fromPlayer);
	bool isFromPlayer();

protected:
	virtual void calculateBB();
	virtual void collideMapRight();
	virtual void collideMapLeft();
	virtual void collideMapTop();
	virtual void collideMapBottom();
	virtual bool collideWithMap(int direction);

	virtual void onDying();
	virtual void stuck();

	int damages;
	float renderScale;
	enumShotType MeleeHit;
	enemyTypeEnum enemyType;

	ParticleGenerator particleGenerator;

private:
	bool testWallsCollision;
	bool flying;
	bool critical;
	bool goThrough;
	unsigned int level;

	int hitNumber;
	bool fromPlayer;

	
	~MeleeRuler();
};

