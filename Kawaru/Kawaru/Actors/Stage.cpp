#include "Stage.h"
#include "../Systems/Input.h"

Stage::Stage(NavMesh& navMesh):
	Actor(L"Models/map.mqo"), navMesh_(navMesh)
{
}

Stage::~Stage()
{
}

void Stage::Update(const Input& input)
{
	MV1SetPosition(modelHandle_, pos_);
}

void Stage::Draw()
{
	MV1DrawModel(modelHandle_);
}

NavMesh& Stage::GetNavMesh() const
{
	return navMesh_;
}
