#include "Ghost.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "../NavMesh/NavMeshMoveComponent.h"

namespace
{
	// 移動速度
	constexpr float DEFAULT_MOVE_SPEED = 50.0f;

	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 150.0f;

	// 当たり判定カプセルの高さ
	constexpr float HIT_HEIGHT = 150.0f;

	// 目標地点に達したとみなす半径
	constexpr float GOAL_REACH_RADIUS = 305.0f;
}

Ghost::Ghost(const Stage& stage, const float posX, const float posY, const float posZ):
	Character(stage, HIT_WIDTH * 4, HIT_HEIGHT * 2, posX, posY, posZ)
{
	floatingOffset_ = VGet(0.0f, 300.0f, 0.0f);
	navMeshMoveComponent_ = std::make_unique<NavMeshMoveComponent>(*this, stage_);
}

Ghost::~Ghost()
{
}

void Ghost::Update(const Input& input)
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

void Ghost::Draw()
{
	DrawShadow();

	DrawSphere3D(VAdd(pos_, VGet(0.0f, HIT_WIDTH * 2, 0.0f)), HIT_WIDTH * 2, 10, 0xff0000, 0xff0000, false);

	navMeshMoveComponent_->Draw();
}
