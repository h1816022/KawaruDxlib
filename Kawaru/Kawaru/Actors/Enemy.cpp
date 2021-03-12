#include "Enemy.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "NavMesh.h"
#include "../NavMeshPath.h"
#include "../NavMeshMoveComponent.h"

namespace
{
	// �ړ����x
	constexpr float DEFAULT_MOVE_SPEED = 15.0f;

	// �����蔻��J�v�Z���̔��a
	constexpr float HIT_WIDTH = 150.0f;

	// �����蔻��J�v�Z���̍���
	constexpr float HIT_HEIGHT = 680.0f;

	// �̂̕�
	constexpr float BODY_WIDTH = HIT_WIDTH / 3 * 2;

	// �ڕW�n�_�ɒB�����Ƃ݂Ȃ����a
	constexpr float GOAL_REACH_RADIUS = 305.0f;
}

Enemy::Enemy(const Stage& stage, const float posX, const float posY, const float posZ):
	Character(L"Models/DxChara.x", L"Models/Hatune/Hatune", stage, HIT_WIDTH, HIT_HEIGHT, posX, posY, posZ)
{
	floatingOffset_ = VGet(0.0f, 300.0f, 0.0f);
	navMeshMoveComponent_ = std::make_unique<NavMeshMoveComponent>(*this, stage_);
}

Enemy::~Enemy()
{
}

void Enemy::Update(const Input& input)
{
	if (navMeshMoveComponent_->CheckPathExists())
	{
		auto paths = navMeshMoveComponent_->GetPaths();

		moveDirection_ = VNorm(VSub(VAdd(navMeshMoveComponent_->GetNextTargetPos(), floatingOffset_), pos_));
		moveVec_ = VScale(moveDirection_, DEFAULT_MOVE_SPEED);
	}
	else
	{
		moveVec_ = VGet(0.0f, 0.0f, 0.0f);

		navMeshMoveComponent_->CalcPath(pos_, VAdd(pos_, VGet(-2000.0f, 0.0f, 3500.0f)));
	}

	UpdatePos(moveVec_);
	UpdateAngle();
	UpdateAnim();

	navMeshMoveComponent_->Update(GOAL_REACH_RADIUS);
}

void Enemy::Draw()
{
	MV1DrawModel(modelHandle_);

	DrawShadow();

	navMeshMoveComponent_->Draw();
}