#include <DxLib.h>
#include "Collision.h"
#include "Actors/Actor.h"

Collision::Collision(std::shared_ptr<Actor> owner):
	owner_(owner)
{
}

Collision::~Collision()
{
}

bool Collision::CheckHit(const VECTOR& pos1, const VECTOR& pos2, float radius)
{
	return owner_->CheckHitCapsule(pos1, pos2, radius).HitNum == 0;
}

void Collision::Draw()
{
}
