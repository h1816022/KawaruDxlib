#include "Gimmick.h"
#include "Actor.h"
#include "../Scenes/Scene.h"

Gimmick::Gimmick(Scene& scene, const wchar_t* modelFilePath, const wchar_t* targetTag):
	Actor(scene, modelFilePath), scene_(scene), targetTag_(targetTag)
{
}

Gimmick::~Gimmick()
{
}

void Gimmick::Update(const Input& input)
{
	auto actors = scene_.GetActors(targetTag_);

	for (auto actor : actors)
	{
		auto coll = actor->GetCollisionCapsule(actor->GetPos());
		auto result = CheckHitCapsule(coll.pos1, coll.pos2, coll.radius);

		if (result.HitNum != 0)
		{
			Hit(actor);
		}
	}
}