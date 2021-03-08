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

DxLib::MV1_COLL_RESULT_POLY Actor::CheckHitLine(const VECTOR& start, const VECTOR& end) const
{
	return MV1CollCheck_Line(modelHandle_, -1, start, end);
}

DxLib::MV1_COLL_RESULT_POLY_DIM Actor::CheckHitSphere(const VECTOR& center, float radius)const
{
	return MV1CollCheck_Sphere(modelHandle_, -1, center, radius);
}

DxLib::MV1_COLL_RESULT_POLY_DIM Actor::CheckHitCapsule(const VECTOR& pos1, const VECTOR& pos2, float radius) const
{
	return MV1CollCheck_Capsule(modelHandle_, -1, pos1, pos2, radius);
}