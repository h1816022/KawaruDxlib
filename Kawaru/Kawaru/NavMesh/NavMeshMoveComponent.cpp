#include "NavMeshMoveComponent.h"
#include "../Actors/Stage.h"
#include "NavMesh.h"
#include "NavMeshPath.h"
#include "../Actors/Actor.h"

NavMeshMoveComponent::NavMeshMoveComponent(const Actor& owner, const Stage& stage, NAV_TYPE type):
	owner_(owner), stage_(stage), type_(type)
{
}

NavMeshMoveComponent::~NavMeshMoveComponent()
{
}

bool NavMeshMoveComponent::CalcPath(const VECTOR& startPos, const VECTOR& goalPos)
{
	NavMesh& navMesh = stage_.GetNavMesh();

	constexpr float SEARCH_HEIGHT = 5000.0f;
	auto nowPos = navMesh.CheckHitLine(VAdd(startPos, VGet(0.0f, SEARCH_HEIGHT, 0.0f)), VSub(startPos, VGet(0.0f, SEARCH_HEIGHT, 0.0f)));
	auto goal = navMesh.CheckHitLine(VAdd(goalPos, VGet(0.0f, SEARCH_HEIGHT, 0.0f)), VSub(goalPos, VGet(0.0f, SEARCH_HEIGHT, 0.0f)));

	if (!nowPos.HitFlag || !goal.HitFlag)
	{
		return false;
	}

	NavMeshPath path;
	if (!navMesh.FindPath(path, type_, nowPos.PolygonIndex, nowPos.HitPosition, goal.PolygonIndex, goal.HitPosition))
	{
		return false;
	}

	constexpr float CORNER_OFFSET_RATIO = 0.5f;
	paths_ = path.GetStraightPath(CORNER_OFFSET_RATIO);

	for (int i = 1; i < paths_.size() - 1; ++i)
	{
		paths_[i].y = max(paths_[i].y, paths_[i + 1].y);
	}

	return true;
}

const std::vector<VECTOR> NavMeshMoveComponent::GetPaths() const
{
	return paths_;
}

void NavMeshMoveComponent::Draw() const
{
	if (paths_.size() != 0)
	{
		for (int i = 0; i < (paths_.size() - 1); ++i)
		{
			DrawLine3D(VAdd(paths_[i], VGet(0.0f, 0.0f, 0.0f)), VAdd(paths_[i + 1], VGet(0.0f, 0.0f, 0.0f)), 0x00ff00);
		}
	}
}

bool NavMeshMoveComponent::CheckPathExists() const
{
	return paths_.size() >= 2;
}

VECTOR NavMeshMoveComponent::GetNextTargetPos()
{
	if (CheckPathExists())
	{
		return paths_[1];
	}
	else
	{
		VGet(0.0f, 0.0f, 0.0f);
	}
}

bool NavMeshMoveComponent::ReachToGoal(float radius) const
{
	if (!CheckPathExists())
	{
		return false;
	}

	VECTOR diff = VSub(paths_[1], owner_.GetPos());
	return (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) <= (radius * radius);
}

void NavMeshMoveComponent::Update(float goalReachradius)
{
	if (ReachToGoal(goalReachradius))
	{
		paths_.erase(paths_.begin());
	}

	if (!CheckPathExists())
	{
		return;
	}

	if (paths_[0].y < paths_[1].y)
	{
		auto result = stage_.CheckHitLine(VAdd(paths_[1], VGet(0.0f, 1.0f, 0.0f)), VAdd(owner_.GetPos(), VGet(0.0f, 1.0f, 0.0f)));
		if (!result.HitFlag)
		{
			return;
		}

		VECTOR highestPos = result.Position[0];
		for (int index = 1; index < 3; ++index)
		{
			if (highestPos.y < result.Position[index].y)
			{
				highestPos = result.Position[index];
			}
		}

		if (paths_.size() == 2)
		{
			paths_.emplace(paths_.begin() + 1, highestPos);
		}
		else
		{
			paths_[1] = highestPos;
		}
	}
}
