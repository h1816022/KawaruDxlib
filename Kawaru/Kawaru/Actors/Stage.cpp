#include "Stage.h"
#include "../Systems/Input.h"

Stage::Stage():
	Actor(L"Models/TestMap.mqo", 500.0f, 0.0f, 0.0f)
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