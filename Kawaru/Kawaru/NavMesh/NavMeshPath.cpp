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
	// �ŏI�I�Ȍo�H���W���X�g
	std::vector<VECTOR> result;

	cornerOffsetRatio = std::clamp(cornerOffsetRatio, 0.0f, 1.0f);

	// ���L����Ӄ��X�g
	std::vector<CellEdge> portalEdges;

	// �T�����̍��E�̒��_���W
	VECTOR left;
	VECTOR right;

	// �n�_�̓o�^
	left = right = waypoints_[0].pos;
	portalEdges.emplace_back(CellEdge(left, right));

	// �[�ɓ�����n�_�ƏI�_�ȊO��T��
	for (int i = 1; i < waypoints_.size() - 1; ++i)
	{
		// �e�Z��(1�S�[���ɋ߂��Z��)�Ƌ��L����ӂ���A���E���W���X�V���Ӄ��X�g�ɒǉ��o�^
		left = waypoints_[i].arrivalEdge.left;
		right = waypoints_[i].arrivalEdge.right;

		portalEdges.emplace_back(CellEdge(left, right));
	}

	// �I�_�̓o�^
	left = right = waypoints_[waypoints_.size() - 1].pos;
	portalEdges.emplace_back(CellEdge(left, right));

	// ��_�ƁA�������猩�č��E�̓_
	VECTOR portalApex;
	VECTOR portalLeft;
	VECTOR portalRight;

	int apexIndex = 0;
	int leftIndex = 0;
	int rightIndex = 0;

	portalApex = portalEdges[0].left;
	portalLeft = portalEdges[0].left;
	portalRight = portalEdges[0].right;

	// �n�_���ŏI�I�Ȍo�H�ɂ܂��o�^
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
		// ���̃G�b�W�̌��
		nextLeft = portalEdges[i].left;
		nextRight = portalEdges[i].right;

		// �u���̉E�̓_���v���u���̉E�̓_�v��荶���ɂ���Ȃ�A�i�߂�Όo�H�̃W�O�U�O���ɘa�����̂Ői�߂�
		if (IsTightened(NavMeshMath::POINT_SIDE::RightSide))
		{
			// ��_����u���̍��̓_�v�ւ̃x�N�g���Ɣ�ׁA�u���̉E�̓_���v�����E�t�]�����ʒu�ɖ��������`�F�b�N
			if (!IsCrossed(NavMeshMath::POINT_SIDE::RightSide))
			{
				// �E�̓_��i�߂�
				portalRight = nextRight;
				rightIndex = i;
			}
			// �ǂ��z�������������ꍇ
			else
			{
				// �ǂ��z���ꂽ�_���A�R�[�i�[���炸�炷���������ɕ␳
				portalLeft = VAdd(VScale(portalLeft, (1.0f - cornerOffsetRatio)), VScale(portalEdges[leftIndex].right, cornerOffsetRatio));

				// �o�H�ɒǉ�
				result.emplace_back(portalLeft);
				
				// �ǂ��z���ꂽ�_��V���Ȋ�_�Ƃ���
				portalApex = portalLeft;
				apexIndex = leftIndex;

				// �n�߂̂Ƃ��̂悤�Ƀ��Z�b�g
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				i = apexIndex;

				continue;
			}
		}

		// �E�Ɠ����悤�Ƀ`�F�b�N
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

	// �I�_�̒ǉ�
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
