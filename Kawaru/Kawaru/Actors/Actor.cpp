#include "Actor.h"

Actor::Actor(const float posX, const float posY, const float posZ)
{
	Init(posX, posY, posZ);
}

Actor::Actor(const wchar_t* modelFilePath, const float posX, const float posY, const float posZ)
{
	modelHandle_ = MV1LoadModel(modelFilePath);
	Init(posX, posY, posZ);
}

Actor::~Actor()
{
	MV1DeleteModel(modelHandle_);
}

void Actor::Init(const float posX, const float posY, const float posZ)
{
	pos_ = VGet(posX, posY, posZ);
	scale_ = VGet(1.0f, 1.0f, 1.0f);
}

void Actor::Draw()
{
}

const VECTOR& Actor::GetPos() const
{
	return pos_;
}
