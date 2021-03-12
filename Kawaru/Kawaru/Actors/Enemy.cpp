#include "Enemy.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "NavMesh.h"

namespace
{
	// 移動速度
	constexpr float DEFAULT_MOVE_SPEED = 30.0f;

	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 150.0f;

	// 当たり判定カプセルの高さ
	constexpr float HIT_HEIGHT = 680.0f;

	// 体の幅
	constexpr float BODY_WIDTH = HIT_WIDTH / 3 * 2;

	// 目標地点に達したとみなす半径
	constexpr float GOAL_REACH_RADIUS = 100.0f;
}

Enemy::Enemy(const Stage& stage, const float posX, const float posY, const float posZ):
	Character(L"Models/DxChara.x", L"Models/Hatune/Hatune", stage, HIT_WIDTH, HIT_HEIGHT, posX, posY, posZ)
{
	
}

Enemy::~Enemy()
{
}

void Enemy::Update(const Input& input)
{
	if (CheckPath())
	{
		moveDirection_ = VNorm(VSub(paths_[1], pos_));
		moveVec_ = VScale(moveDirection_, DEFAULT_MOVE_SPEED);
	}
	else
	{
		moveVec_ = VGet(0.0f, 0.0f, 0.0f);

		NavMesh& navMesh = stage_.GetNavMesh();

		auto nowPos = navMesh.CheckHitLine(VAdd(pos_, VGet(0.0f, 5000.0f, 0.0f)), VSub(pos_, VGet(0.0f, 5000.0f, 0.0f)));

		auto goal = navMesh.CheckHitLine(VAdd(pos_, VGet(2000.0f, 5000.0f, 4000.0f)), VSub(pos_, VGet(-2000.0f, 5000.0f, -4000.0f)));

		if (nowPos.HitFlag && goal.HitFlag)
		{
			goalPos[0] = goal.Position[0];
			goalPos[1] = goal.Position[1];
			goalPos[2] = goal.Position[2];

			NavMeshPath path;
			if (navMesh.FindPath(path, nowPos.PolygonIndex, nowPos.HitPosition, goal.PolygonIndex, goal.HitPosition))
			{
				paths_ = path.GetStraightPath(0.5f);
			}
		}
	}

	UpdatePos(moveVec_);
	UpdateAngle();
	UpdateAnim();

	if (ReachToGoal())
	{
		UpdatePaths();
	}
}

void Enemy::Draw()
{
	if (paths_.size() != 0)
	{
		for (int i = 0; i < (paths_.size() - 1); ++i)
		{
			DrawLine3D(VAdd(paths_[i], VGet(0.0f, 0.0f, 0.0f)), VAdd(paths_[i + 1], VGet(0.0f, 0.0f, 0.0f)), 0x00ff00);
		}

		DrawLine3D(goalPos[0], goalPos[1], 0x00ffff);
		DrawLine3D(goalPos[1], goalPos[2], 0x00ffff);
		DrawLine3D(goalPos[2], goalPos[0], 0x00ffff);
	}

	MV1DrawModel(modelHandle_);

	DrawShadow();
}

bool Enemy::CheckPath() const
{
	return (paths_.size() > 1);
}

bool Enemy::ReachToGoal()const
{
	if (!CheckPath())
	{
		return false;
	}

	VECTOR diff = VSub(paths_[1], pos_);
	return (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) <= GOAL_REACH_RADIUS  * GOAL_REACH_RADIUS;
}

void Enemy::UpdatePaths()
{
	paths_.erase(paths_.begin());
}
