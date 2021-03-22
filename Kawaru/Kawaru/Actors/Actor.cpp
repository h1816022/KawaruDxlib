#include "Actor.h"
#include "../Scenes/Scene.h"

Actor::Actor(Scene& scene, const float posX, const float posY, const float posZ):
	scene_(scene)
{
	Init(L"", L"", posX, posY, posZ);
}

Actor::Actor(Scene& scene, const wchar_t* modelFilePath, const float posX, const float posY, const float posZ):
	scene_(scene)
{
	modelHandle_ = MV1LoadModel(modelFilePath);
	Init(modelFilePath, L"", posX, posY, posZ);
}

Actor::Actor(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX, const float posY, const float posZ):
	scene_(scene)
{
	MV1SetLoadModelAnimFilePath(motionFilePath);
	modelHandle_ = MV1LoadModel(modelFilePath);
	Init(modelFilePath, motionFilePath, posX, posY, posZ);
}

Actor::~Actor()
{
	MV1DeleteModel(modelHandle_);
}

void Actor::Init(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX, const float posY, const float posZ)
{
	pos_ = VGet(posX, posY, posZ);

	scale_ = VGet(1.0f, 1.0f, 1.0f);

	MV1SetLoadModelAnimFilePath(motionFilePath);
	modelHandle_ = MV1LoadModel(modelFilePath);

	MV1SetPosition(modelHandle_, pos_);
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

const std::wstring& Actor::GetTag() const
{
	return tag_;
}

Capsule3D Actor::GetCollisionCapsule(const VECTOR& pos)
{
	return Capsule3D();
}

bool Actor::Destroy()
{
	if (isDead_)
	{
		return false;
	}

	isDead_ = true;

	return true;
}

bool Actor::CheckIsDead() const
{
	return isDead_;
}