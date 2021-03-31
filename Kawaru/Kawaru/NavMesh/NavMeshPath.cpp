#include <algorithm>
#include <cassert>
#include "NavMeshPath.h"
#include "NavMeshCells.h"
#include "../Tools/Geometry.h"

struct NavMeshMath
{
	enum class POINT_SIDE
	{
		LeftSide,
		RightSide,
		OnLine
	};

	static POINT_SIDE GetClassifyPoint(const VECTOR& A, const VECTOR& B, const VECTOR& C)
	{
		Vector2f ab(B.x - A.x, B.y - A.y);
		Vector2f ac(C.x - A.x, C.y - A.y);

		float value = Cross(ab.Normalized(), ac.Normalized());

		if (value == 0)
		{
			return POINT_SIDE::OnLine;
		}

		if (value < 0)
		{
			return POINT_SIDE::LeftSide;
		}
		else
		{
			return POINT_SIDE::RightSide;
		}
	}
};

NavMeshPath::NavMeshPath()
{
}

NavMeshPath::~NavMeshPath()
{
}

void NavMeshPath::AddWaypoint(const VECTOR& pos)
{
	waypoints_.emplace_back(Waypoint(pos));
}

void NavMeshPath::AddWaypoint(NAV_TYPE type, std::shared_ptr<NavMeshCells> cell, std::shared_ptr<NavMeshCells> parent)
{
	waypoints_.emplace_back(cell->GetCenter(), cell->GetID(), cell->GetArrivalEdge(type, parent->GetID()));
}

const std::vector<Waypoint>& NavMeshPath::GetWaypoints() const
{
	return waypoints_;
}

std::vector<VECTOR> NavMeshPath::GetStraightPath(float cornerOffsetRatio) const
{
	// 最終的な経路座標リスト
	std::vector<VECTOR> result;

	cornerOffsetRatio = std::clamp(cornerOffsetRatio, 0.0f, 1.0f);

	// 共有する辺リスト
	std::vector<CellEdge> portalEdges;

	// 探索中の左右の頂点座標
	VECTOR left;
	VECTOR right;

	// 始点の登録
	left = right = waypoints_[0].pos;
	portalEdges.emplace_back(CellEdge(left, right));

	// 端に当たる始点と終点以外を探索
	for (int i = 1; i < waypoints_.size() - 1; ++i)
	{
		// 親セル(1つゴールに近いセル)と共有する辺から、左右座標を更新し辺リストに追加登録
		left = waypoints_[i].arrivalEdge.left;
		right = waypoints_[i].arrivalEdge.right;

		portalEdges.emplace_back(CellEdge(left, right));
	}

	// 終点の登録
	left = right = waypoints_[waypoints_.size() - 1].pos;
	portalEdges.emplace_back(CellEdge(left, right));

	// 基準点と、そこから見て左右の点
	VECTOR portalApex;
	VECTOR portalLeft;
	VECTOR portalRight;

	int apexIndex = 0;
	int leftIndex = 0;
	int rightIndex = 0;

	portalApex = portalEdges[0].left;
	portalLeft = portalEdges[0].left;
	portalRight = portalEdges[0].right;

	// 始点を最終的な経路にまず登録
	result.emplace_back(portalApex);

	VECTOR nextLeft;
	VECTOR nextRight;

	auto IsTightened = [&portalApex, &portalLeft, &portalRight, &nextLeft, &nextRight](NavMeshMath::POINT_SIDE side)
	{
		bool ret = false;

		switch (side)
		{
		case NavMeshMath::POINT_SIDE::OnLine:
			assert(false);
			break;

		case NavMeshMath::POINT_SIDE::LeftSide:
			ret = NavMeshMath::GetClassifyPoint(portalApex, portalLeft, nextLeft) != NavMeshMath::POINT_SIDE::LeftSide;
			break;

		case NavMeshMath::POINT_SIDE::RightSide:
			ret = NavMeshMath::GetClassifyPoint(portalApex, portalRight, nextRight) != NavMeshMath::POINT_SIDE::RightSide;
			break;
		}

		return ret;
	};

	auto IsCrossed = [&portalApex, &portalLeft, &portalRight, &nextLeft, &nextRight](NavMeshMath::POINT_SIDE side)
	{
		bool ret = false;

		switch (side)
		{
		case NavMeshMath::POINT_SIDE::OnLine:
			assert(false);
			break;

		case NavMeshMath::POINT_SIDE::LeftSide:
			ret = !CheckMatch(portalApex, portalLeft) && NavMeshMath::GetClassifyPoint(portalApex, portalRight, nextLeft) != NavMeshMath::POINT_SIDE::LeftSide;
			break;

		case NavMeshMath::POINT_SIDE::RightSide:
			ret = !CheckMatch(portalApex, portalRight) && NavMeshMath::GetClassifyPoint(portalApex, portalLeft, nextRight) != NavMeshMath::POINT_SIDE::RightSide;
			break;
		}

		return ret;
	};

	for (int i = 1; i < portalEdges.size(); ++i)
	{
		// 次のエッジの候補
		nextLeft = portalEdges[i].left;
		nextRight = portalEdges[i].right;

		// 「次の右の点候補」が「今の右の点」より左側にあるなら、進めれば経路のジグザグが緩和されるので進める
		if (IsTightened(NavMeshMath::POINT_SIDE::RightSide))
		{
			// 基準点から「今の左の点」へのベクトルと比べ、「次の右の点候補」が左右逆転した位置に無いかをチェック
			if (!IsCrossed(NavMeshMath::POINT_SIDE::RightSide))
			{
				// 右の点を進める
				portalRight = nextRight;
				rightIndex = i;
			}
			// 追い越しが発生した場合
			else
			{
				// 追い越された点を、コーナーからずらす割合を元に補正
				portalLeft = VAdd(VScale(portalLeft, (1.0f - cornerOffsetRatio)), VScale(portalEdges[leftIndex].right, cornerOffsetRatio));

				// 経路に追加
				result.emplace_back(portalLeft);
				
				// 追い越された点を新たな基準点とする
				portalApex = portalLeft;
				apexIndex = leftIndex;

				// 始めのときのようにリセット
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				i = apexIndex;

				continue;
			}
		}

		// 右と同じようにチェック
		if (IsTightened(NavMeshMath::POINT_SIDE::LeftSide))
		{
			if (!IsCrossed(NavMeshMath::POINT_SIDE::LeftSide))
			{
				portalLeft = nextLeft;
				leftIndex = i;
			}
			else
			{
				portalRight = VAdd(VScale(portalRight, (1.0f - cornerOffsetRatio)), VScale(portalEdges[rightIndex].left, cornerOffsetRatio));

				result.emplace_back(portalRight);

				portalApex = portalRight;
				apexIndex = rightIndex;

				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;

				i = apexIndex;
				continue;
			}
		}
	}

	// 終点の追加
	if (result.size() == 0 || !CheckMatch(result[result.size() - 1], portalEdges[portalEdges.size() - 1].left))
	{
		result.emplace_back(waypoints_[waypoints_.size() - 1].pos);
	}

	return result;
}

Waypoint::Waypoint(const VECTOR& pos):
	pos(pos), id(-1), arrivalEdge(CellEdge())
{
}

Waypoint::Waypoint(const VECTOR& pos, int id, const CellEdge& edge):
	pos(pos), id(id), arrivalEdge(edge)
{
}
