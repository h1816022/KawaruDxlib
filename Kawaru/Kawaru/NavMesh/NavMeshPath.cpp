#include <algorithm>
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

	static POINT_SIDE ClassifyPoint(const VECTOR& A, const VECTOR& B, const VECTOR& C)
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
	std::vector<VECTOR> result;

	cornerOffsetRatio = std::clamp(cornerOffsetRatio, 0.0f, 1.0f);

	std::vector<CellEdge> portalEdges;

	VECTOR left;
	VECTOR right;

	left = right = waypoints_[0].pos;

	portalEdges.emplace_back(CellEdge(left, right));

	for (int i = 1; i < waypoints_.size() - 1; ++i)
	{
		left = waypoints_[i].arrivalEdge.left;
		right = waypoints_[i].arrivalEdge.right;

		portalEdges.emplace_back(CellEdge(left, right));
	}

	left = right = waypoints_[waypoints_.size() - 1].pos;
	portalEdges.emplace_back(CellEdge(left, right));

	VECTOR portalApex;
	VECTOR portalLeft;
	VECTOR portalRight;

	int apexIndex = 0;
	int leftIndex = 0;
	int rightIndex = 0;

	portalApex = portalEdges[0].left;
	portalLeft = portalEdges[0].left;
	portalRight = portalEdges[0].right;

	result.emplace_back(portalApex);

	for (int i = 1; i < portalEdges.size(); ++i)
	{
		VECTOR nextLeft = portalEdges[i].left;
		VECTOR nextRight = portalEdges[i].right;

		NavMeshMath::POINT_SIDE classification = NavMeshMath::ClassifyPoint(portalApex, portalRight, nextRight);

		if (classification != NavMeshMath::POINT_SIDE::RightSide)
		{
			classification = NavMeshMath::ClassifyPoint(portalApex, portalLeft, nextRight);

			if (CheckMatch(portalApex, portalRight) || classification == NavMeshMath::POINT_SIDE::RightSide)
			{
				portalRight = nextRight;
				rightIndex = i;
			}
			else
			{
				portalLeft = VAdd(VScale(portalLeft, (1.0f - cornerOffsetRatio)), VScale(portalEdges[leftIndex].right, cornerOffsetRatio));

				result.emplace_back(portalLeft);

				portalApex = portalLeft;
				apexIndex = leftIndex;

				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;

				i = apexIndex;
				continue;
			}
		}

		classification = NavMeshMath::ClassifyPoint(portalApex, portalLeft, nextLeft);

		if (classification != NavMeshMath::POINT_SIDE::LeftSide)
		{
			classification = NavMeshMath::ClassifyPoint(portalApex, portalRight, nextLeft);

			if (CheckMatch(portalApex, portalLeft) || classification == NavMeshMath::POINT_SIDE::LeftSide)
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
