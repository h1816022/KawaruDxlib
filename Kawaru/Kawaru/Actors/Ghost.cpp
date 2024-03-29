#include "Ghost.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "../NavMesh/NavMeshMoveComponent.h"
#include "../NavMesh/NavMesh.h"
#include "Camera.h"
#include "../Tools/Geometry.h"
#include "../Scenes/Scene.h"
#include "../Scenes/GameplayingScene.h"
#include "../Systems/File.h"
#include "../Systems/FileManager.h"

namespace
{
	// 移動速度
	constexpr float MAX_MOVE_SPEED = 80.0f;

	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 12.0f;

	// 目標地点に達したとみなす半径のオフセット
	constexpr float GOAL_REACH_RADIUS_OFFSET = 150.0f;

	// 近づけたとする距離
	constexpr float APPROACH_DISTANCE = 2000.0f;

	// 浮くためのオフセット値
	constexpr float FLOATING_OFFSET = 500.0f;

	// ふわふわする距離
	constexpr float FLOAT_LENGTH = 15.0f;

	// ふわふわする時間
	constexpr int FLOAT_DURATION = 60;
}

Ghost::Ghost(Scene& scene, Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(scene, stage, HIT_WIDTH, HIT_WIDTH, posX, posY, posZ), camera_(camera),
	floatingOffset_(FLOATING_OFFSET),
	moveSpeed_(0.0f)
{
	navMeshMoveComponent_ = std::make_unique<NavMeshMoveComponent>(*this, stage_, NAV_TYPE::Floated);

	oldMoveDirection_ = VGet(0.0f, 0.0f, 0.0f);

	auto& fileManager = FileManager::Instance();

	moveSE_ = fileManager.Load(L"Resources/Sounds/GhostMove.mp3")->GetHandle();
	ChangeVolumeSoundMem(128, moveSE_);

	collisionEnabled_ = false;
}

Ghost::~Ghost()
{
}

void Ghost::Update(const Input& input)
{
	if (gameEnd_)
	{
		constexpr float LERP_RATE = 0.01f;
		SetPos(Lerp(pos_, goalPos_, LERP_RATE));
		camera_.SetPos(VAdd(pos_, VGet(0.0f, floatingOffset_, 0.0f)));
	}
	else
	{
		if (navMeshMoveComponent_->CheckPathExists())
		{
			oldMoveDirection_ = moveDirection_;
			moveDirection_ = VNorm(VSub(VAdd(navMeshMoveComponent_->GetNextTargetPos(), VGet(0.0f, FLOATING_OFFSET + GOAL_REACH_RADIUS_OFFSET, 0.0f)), pos_));

			constexpr float DIR_LERP_RATE = 0.1f;
			moveDirection_ = Lerp(oldMoveDirection_, moveDirection_, DIR_LERP_RATE);
			
			constexpr float MOVESPEED_LERP_RATE = 0.005f;
			moveSpeed_ = Lerp(moveSpeed_, MAX_MOVE_SPEED, MOVESPEED_LERP_RATE);
			moveVec_ = VScale(moveDirection_, moveSpeed_);
		}
		else
		{
			EndMove();

			moveVec_ = VGet(0.0f, 0.0f, 0.0f);
		}

		UpdatePos(moveVec_);
		UpdateAngle();

		navMeshMoveComponent_->Update(FLOATING_OFFSET + GOAL_REACH_RADIUS_OFFSET + HIT_WIDTH);

		camera_.SetPos(VAdd(pos_, VGet(0.0f, floatingOffset_, 0.0f)));

		const float FLOATING_LERP_RATE = static_cast<float>(GetRand(3) + 2) / 100.0f;
		floatingOffset_ = Lerp(floatingOffset_, FLOATING_OFFSET + FLOAT_LENGTH * ((floatingCount_ < FLOAT_DURATION / 2) ? 1.0f : -1.0f), FLOATING_LERP_RATE);
		floatingCount_ = (++floatingCount_ > FLOAT_DURATION) ? 0 : floatingCount_;
	}
}

void Ghost::Draw()
{
	DrawShadow();
}

bool Ghost::Call()
{
	if (moveFlag_)
	{
		return false;
	}

	ApproachPlayer();

	PlaySoundMem(moveSE_, DX_PLAYTYPE_BACK);

	return true;
}

void Ghost::EndGame()
{
	gameEnd_ = true;

	goalPos_ = CalcApproachPos(camera_.GetTargetPos());
}

void Ghost::InitPos()
{
	SetPos(CalcApproachPos(camera_.GetTargetPos()));
}

void Ghost::ApproachPlayer()
{
	VECTOR goalPos;
	if (camera_.GetVisiblePlayerPos(goalPos))
	{
		navMeshMoveComponent_->CalcPath(pos_, CalcApproachPos(goalPos));

		moveFlag_ = true;

		followingPlayer_ = true;
	}
	else
	{
		navMeshMoveComponent_->CalcPath(pos_, CalcApproachPos(camera_.GetTargetPos()));
		followingPlayer_ = false;
		moveFlag_ = true;
	}
}

void Ghost::ApproachTarget(const VECTOR& target)
{
	navMeshMoveComponent_->CalcPath(pos_, target);

	moveFlag_ = true;
}

VECTOR Ghost::CalcApproachPos(const VECTOR& target)
{
	VECTOR ret;

	while (true)
	{
		VECTOR dir = RandomVector();
		dir = VGet(dir.x, abs(dir.y), dir.z);

		ret = VAdd(target, VScale(dir, APPROACH_DISTANCE));

		auto result = stage_.CheckHitLine(target, ret);

		if (!result.HitFlag)
		{
			break;
		}

		constexpr float THRESHOLD = APPROACH_DISTANCE / 2.0f;

		if (GetLength(VSub(result.HitPosition, target)) < THRESHOLD)
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

	StopSoundMem(moveSE_);
}