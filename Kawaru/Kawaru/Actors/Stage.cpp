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