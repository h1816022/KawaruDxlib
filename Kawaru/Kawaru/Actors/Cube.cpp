#include "Cube.h"
#include "../Systems/Input.h"

namespace
{
	constexpr float DEFAULT_SIZE = 100.0f;
}

Cube::Cube(const float posX, const float posY, const float posZ, const float scaleX, const float scaleY, const float scaleZ):
	Actor(posX, posY, posZ), scale_(VGet(scaleX, scaleY, scaleZ))
{
}

Cube::~Cube()
{
}

void Cube::Update(const Input& input)
{
}

void Cube::Draw()
{
	auto pos1 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z + DEFAULT_SIZE / 2);
	auto pos2 = VGet(pos_.x + DEFAULT_SIZE / 2, pos_.y, pos_.z + DEFAULT_SIZE / 2);
	auto pos3 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	auto pos4 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	auto pos5 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	auto pos6 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	auto pos7 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	auto pos8 = VGet(pos_.x - DEFAULT_SIZE / 2, pos_.y, pos_.z - DEFAULT_SIZE / 2);
	
	DrawTriangle3D(pos1, pos2, pos3, 0xffffff, true);
	DrawTriangle3D(pos1, pos3, pos2, 0xffffff, true);
}