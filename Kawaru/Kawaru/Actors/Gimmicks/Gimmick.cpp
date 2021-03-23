#include "Gimmick.h"
#include "../Actor.h"
#include "../../Scenes/Scene.h"

Gimmick::Gimmick(Scene& scene, const wchar_t* modelFilePath, const wchar_t* targetTag, bool onceHit, float px, float py, float pz):
	Actor(scene, modelFilePath, px, py, pz), scene_(scene), targetTag_(targetTag), onceHit_(onceHit)
{
}

Gimmick::~Gimmick()
{
}

void Gimmick::Update(const Input& input)
{
	if ((onceHit_ && hit_) ||
		!collisionEnabled_)
	{
		return;
	}

	auto actors = scene_.GetActors(targetTag_);

	for (auto actor : actors)
	{
		if (CheckHit(actor))
		{
			if (Hit(actor))
			{
				hit_ = true;
			}
		}
	}
}

bool Gimmick::CheckHit(std::shared_ptr<Actor> actor)
{
	auto coll = actor->GetCollisionCapsule(actor->GetPos());
	auto result = CheckHitCapsule(coll.pos1, coll.pos2, coll.radius);

	return result.HitNum != 0;
}
