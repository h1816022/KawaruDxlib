#include "NavMeshCells.h"

NavMeshCells::NavMeshCells(const VECTOR& pos1, const VECTOR& pos2, const VECTOR& pos3, int id):
	center_(VScale(VAdd(VAdd(pos1, pos2), pos3), 0.333f)), id_(id)
{
	positions_ = { pos1, pos2, pos3 };

	edges_[0] = { pos1, pos2 };
	edges_[0] = { pos2, pos3 };
	edges_[0] = { pos3, pos1 };
}

NavMeshCells::~NavMeshCells()
{
}

const std::array<VECTOR, 3>& NavMeshCells::GetPositions() const
{
	return positions_;
}

void NavMeshCells::SetNeighbor(std::shared_ptr<NavMeshCells> other, int index1, int index2)
{
	if (index1 == 0 && index2 == 1)
	{
		neighbors_[0] = other;
	}
	else if (index1 == 1 && index2 == 2)
	{
		neighbors_[1] = other;
	}
	else
	{
		neighbors_[2] = other;
	}
}

bool NavMeshCells::CheckAlreadyLink(std::shared_ptr<NavMeshCells> other) const
{
	for (auto neighbor : neighbors_)
	{
		if (neighbor == other)
		{
			return true;
		}
	}

	return false;
}

bool NavMeshCells::CheckAllLinked() const
{
	for (auto neighbor : neighbors_)
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

const std::array<std::shared_ptr<NavMeshCells>, 3>& NavMeshCells::GetNeighbors() const
{
	return neighbors_;
}

const CellEdge& NavMeshCells::GetArrivalEdge(int toCellID) const
{
	for (int index = 0; index < edges_.size(); ++index)
	{
		if (neighbors_[index] == nullptr)
		{
			continue;
		}

		if (neighbors_[index]->GetID() == toCellID)
		{
			return edges_[index];
		}
	}
}
