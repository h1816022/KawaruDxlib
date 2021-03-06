#include "Plane.h"
#include "../Systems/Input.h"

namespace
{
	constexpr float DEFAULT_SIZE = 100.0f;
}

Plane::Plane(const float posX, const float posY, const float posZ):
	Actor(posX, posY, posZ)
{
	
}

Plane::~Plane()
{
}

void Plane::Update(const Input& input)
{
}

void Plane::Draw()
{
	auto pos1 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z + DEFAULT_SIZE / 2);
	auto pos2 = VGet(pos_.x + DEFAULT_SIZE / 2, pos_.y, pos_.z + DEFAULT_SIZE / 2);
	auto pos3 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	DrawTriangle3D(pos1, pos2, pos3, 0xffffff, true);

	pos1 = VGet(pos_.x + DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	DrawTriangle3D(pos1, pos3, pos2, 0xffffff, true);
}