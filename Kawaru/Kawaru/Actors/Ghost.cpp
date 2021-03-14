#include "Ghost.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "../NavMesh/NavMeshMoveComponent.h"
#include "../NavMesh/NavMesh.h"
#include "Camera.h"
#include "../Geometry.h"

namespace
{
	// 移動速度
	constexpr float DEFAULT_MOVE_SPEED = 150.0f;

	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 15.0f;

	// 当たり判定カプセルの高さ
	constexpr float HIT_HEIGHT = 15.0f;

	// 目標地点に達したとみなす半径
	constexpr float GOAL_REACH_RADIUS = 1250.0f;

	constexpr float APPROACH_DISTANCE = 2000.0f;

}

Ghost::Ghost(Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(stage, HIT_WIDTH * 4, HIT_HEIGHT * 2, posX, posY, posZ), camera_(camera)
{
	floatingOffset_ = VGet(0.0f, 1200.0f, 0.0f);
	navMeshMoveComponent_ = std::make_unique<NavMeshMoveComponent>(*this, stage_, NAV_TYPE::Floated);
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
		EndMove();

		if (input.IsTriggered("DebugMode"))
		{
			ApproachPlayer();
		}
		moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	}

	UpdatePos(moveVec_);
	UpdateAngle();
	UpdateAnim();

	navMeshMoveComponent_->Update(GOAL_REACH_RADIUS);

	camera_.SetPos(pos_);
}

void Ghost::Draw()
{
	DrawShadow();

	navMeshMoveComponent_->Draw();
}

bool Ghost::ApproachPlayer()
{
	VECTOR goalPos;
	if (camera_.GetVisiblePlayerPos(goalPos))
	{
		navMeshMoveComponent_->CalcPath(pos_, CalcApproachPos(goalPos));

		moveFlag_ = true;

		followingPlayer_ = true;

		return true;
	}
	else
	{
		navMeshMoveComponent_->CalcPath(pos_, CalcApproachPos(camera_.GetTargetPos()));
		followingPlayer_ = false;
		moveFlag_ = true;

		return true;
	}

	return false;
}

void Ghost::ApproachTarget(const VECTOR& target)
{
	navMeshMoveComponent_->CalcPath(pos_, target);

	moveFlag_ = true;
}

VECTOR Ghost::CalcApproachPos(const VECTOR& target)
{
	VECTOR ret;

	while (1)
	{
		VECTOR dir = RandomVector();
		dir = VGet(dir.x, abs(dir.y), dir.z);

		ret = VAdd(target, VScale(dir, APPROACH_DISTANCE));

		auto result = stage_.CheckHitLine(target, ret);

		if (!result.HitFlag)
		{
			break;
		}

		if (GetLength(VSub(result.HitPosition, target)) < APPROACH_DISTANCE / 2.0f)
		{
			continue;
		}
	}

	ret = VGet(ret.x, ret.y + floatingOffset_.y, ret.z);

	return ret;
}

void Ghost::EndMove()
{
	if (!moveFlag_)
	{
		return;
	}

	if (!followingPlayer_ && camera_.GetFollowingPlayerFlag())
	{
		auto target = VSub(camera_.GetTargetPos(), floatingOffset_);
		if (GetLength(VSub(pos_, target)) > APPROACH_DISTANCE)
		{
			ApproachTarget(VAdd(target, VScale(VNorm(VSub(pos_, target)), APPROACH_DISTANCE)));
		}
	}

	moveFlag_ = false;
}
