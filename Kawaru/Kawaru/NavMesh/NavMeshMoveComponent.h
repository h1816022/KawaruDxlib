#pragma once
#include <DxLib.h>
#include <vector>

class Actor;
class Stage;
enum class NAV_TYPE;

class NavMeshMoveComponent
{
public:
	NavMeshMoveComponent(const Actor& owner, const Stage& stage, NAV_TYPE type);
	~NavMeshMoveComponent();

	bool CalcPath(const VECTOR& startPos, const VECTOR& goalPos, float minYOffset = 0.0f, float maxYOffset = 0.0f);

	const std::vector<VECTOR> GetPaths()const;

	void Draw()const;

	bool CheckPathExists()const;

	VECTOR GetNextTargetPos();

	bool ReachToGoal(float radius)const;

	void Update(float goalReachradius);

private:
	const Stage& stage_;

	std::vector<VECTOR> paths_;

	const Actor& owner_;

	const NAV_TYPE type_;
};