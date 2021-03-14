#include "NavMeshCells.h"
#include "NavMesh.h"

NavMeshCells::NavMeshCells(const VECTOR& pos1, const VECTOR& pos2, const VECTOR& pos3, int id):
	center_(VScale(VAdd(VAdd(pos1, pos2), pos3), 0.333f)), id_(id)
{
	positions_ = { pos1, pos2, pos3 };

	edges_[0] = { pos1, pos2 };
	edges_[1] = { pos2, pos3 };
	edges_[2] = { pos3, pos1 };
}

NavMeshCells::~NavMeshCells()
{
}

const std::array<VECTOR, 3>& NavMeshCells::GetPositions() const
{
	return positions_;
}

void NavMeshCells::SetNeighbor(NAV_TYPE type, std::shared_ptr<NavMeshCells> other, int index1, int index2)
{
	if (index1 == 0 && index2 == 1)
	{
		GetNeighbors(type)[0] = other;
	}
	else if (index1 == 1 && index2 == 2)
	{
		GetNeighbors(type)[1] = other;
	}
	else
	{
		GetNeighbors(type)[2] = other;
	}
}

bool NavMeshCells::CheckAlreadyLink(NAV_TYPE type, std::shared_ptr<NavMeshCells> other) const
{
	for (auto& neighbor : GetNeighbors(type))
	{
		if (neighbor == other)
		{
			return true;
		}
	}
	return false;
}

bool NavMeshCells::CheckAllLinked(NAV_TYPE type) const
{
	for (auto& neighbor : GetNeighbors(type))
	{
		if (neighbor == nullptr)
		{
			return false;
		}
	}

	return true;
}

int NavMeshCells::GetID() const
{
	return id_;
}

const VECTOR& NavMeshCells::GetCenter() const
{
	return center_;
}

const CellEdge& NavMeshCells::GetArrivalEdge(NAV_TYPE type, int toCellID) const
{
	for (int index = 0; index < edges_.size(); ++index)
	{
		if (GetNeighbors(type)[index] == nullptr)
		{
			continue;
		}

		if (GetNeighbors(type)[index]->GetID() == toCellID)
		{
			return edges_[index];
		}
	}
}

std::array<std::shared_ptr<NavMeshCells>, 3>& NavMeshCells::GetNeighbors(NAV_TYPE type)
{
	if (type == NAV_TYPE::Grounded)
	{
		return neighbors_.randed;
	}
	else
	{
		return neighbors_.floated;
	}
}

const std::array<std::shared_ptr<NavMeshCells>, 3>& NavMeshCells::GetNeighbors(NAV_TYPE type) const
{
	if (type == NAV_TYPE::Grounded)
	{
		return neighbors_.randed;
	}
	else
	{
		return neighbors_.floated;
	}
}