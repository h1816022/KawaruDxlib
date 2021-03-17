#include "Stage.h"
#include "../Systems/Input.h"

Stage::Stage(Scene& scene, NavMesh& navMesh):
	Actor(scene, L"Models/t4.mqo"), navMesh_(navMesh)
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
