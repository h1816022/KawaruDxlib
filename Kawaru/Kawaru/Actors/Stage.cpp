#include "Stage.h"
#include "../Systems/Input.h"

Stage::Stage():
	Actor(L"Models/TestMap.mqo")
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

DxLib::MV1_COLL_RESULT_POLY_DIM Stage::CollCheckSphere(const VECTOR& center, float radius)const
{
	return MV1CollCheck_Sphere(modelHandle_, -1, center, radius);
}

DxLib::MV1_COLL_RESULT_POLY_DIM Stage::CollCheckCapsule(const VECTOR& pos1, const VECTOR& pos2, float radius) const
{
	return MV1CollCheck_Capsule(modelHandle_, -1, pos1, pos2, radius);
}
