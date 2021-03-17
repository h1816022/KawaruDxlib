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
	constexpr float MAX_MOVE_SPEED = 80.0f;

	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 15.0f;

	// 目標地点に達したとみなす半径のオフセット
	constexpr float GOAL_REACH_RADIUS_OFFSET = 150.0f;

	// 近づけたとする距離
	constexpr float APPROACH_DISTANCE = 2000.0f;

	constexpr float FLOATING_OFFSET = 500.0f;

	constexpr float FLOAT_LENGTH = 15.0f;
}

Ghost::Ghost(Scene& scene, Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(scene, stage, HIT_WIDTH * 4, HIT_WIDTH * 2, posX, posY, posZ), camera_(camera),
	floatingOffset_(FLOATING_OFFSET),
	moveSpeed_(0.0f)
{
	navMeshMoveComponent_ = std::make_unique<NavMeshMoveComponent>(*this, stage_, NAV_TYPE::Floated);

	oldMoveDirection_ = VGet(0.0f, 0.0f, 0.0f);
}

Ghost::~Ghost()
{
}

void Ghost::Update(const Input& input)
{
	if (navMeshMoveComponent_->CheckPathExists())
	{
		auto paths = navMeshMoveComponent_->GetPaths();

		oldMoveDirection_ = moveDirection_;
		moveDirection_ = VNorm(VSub(VAdd(navMeshMoveComponent_->GetNextTargetPos(), VGet(0.0f, FLOATING_OFFSET + GOAL_REACH_RADIUS_OFFSET, 0.0f)), pos_));
		moveDirection_ = Lerp(oldMoveDirection_, moveDirection_, 0.05f);
		moveSpeed_ = Lerp(moveSpeed_, MAX_MOVE_SPEED, 0.005f);
		moveVec_ = VScale(moveDirection_, moveSpeed_);
	}
	else
	{
		EndMove();

		moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	}

	UpdatePos(moveVec_);
	UpdateAngle();
	UpdateAnim();

	navMeshMoveComponent_->Update(FLOATING_OFFSET + GOAL_REACH_RADIUS_OFFSET + HIT_WIDTH);

	camera_.SetPos(VAdd(pos_, VGet(0.0f, floatingOffset_, 0.0f)));
	
	floatingOffset_ = Lerp(floatingOffset_, FLOATING_OFFSET + FLOAT_LENGTH * ((floatingCount_ < 30) ? 1.0f : -1.0f), static_cast<float>(GetRand(3) + 2) / 100.0f);
	floatingCount_ = (++floatingCount_ > 60) ? 0 : floatingCount_;
}

void Ghost::Draw()
{
	DrawShadow();

	navMeshMoveComponent_->Draw();

	//DrawFormatString(0, 0, 0xffffff, L"%f", abs(f / FLOAT_LENGTH * 2.0f - 1.0f));
}

bool Ghost::Call()
{
	if (moveFlag_)
	{
		return false;
	}

	ApproachPlayer();

	return true;
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

	ret = VGet(ret.x, ret.y + FLOATING_OFFSET, ret.z);

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
		auto target = VSub(camera_.GetTargetPos(), VGet(0.0f, FLOATING_OFFSET, 0.0f));
		if (GetLength(VSub(pos_, target)) > APPROACH_DISTANCE)
		{
			ApproachTarget(VAdd(target, VScale(VNorm(VSub(pos_, target)), APPROACH_DISTANCE)));
		}
	}

	moveSpeed_ = 0.0f;

	moveFlag_ = false;
}
