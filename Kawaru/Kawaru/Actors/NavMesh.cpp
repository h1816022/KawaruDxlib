#include "NavMesh.h"
#include "../NavMeshCells.h"
#include "../NavMeshPath.h"
#include "../Geometry.h"

NavMesh::NavMesh():
	Actor(L"Models/t5.mqo")
{
	MV1SetupReferenceMesh(modelHandle_, -1, true);
	auto result = MV1GetReferenceMesh(modelHandle_, -1, true);
	
	for (int index = 0; index < result.PolygonNum; ++index)
	{
		auto poly = result.Polygons[index];
		auto vert = result.Vertexs;
		cells_.emplace_back(std::make_shared<NavMeshCells>(vert[poly.VIndex[0]].Position, vert[poly.VIndex[1]].Position, vert[poly.VIndex[2]].Position, index));
	}

	for (auto currentCell : cells_)
	{
		for (auto otherCell : cells_)
		{
			if (currentCell == otherCell)
			{
				continue;
			}

			if (currentCell->CheckAlreadyLink(otherCell))
			{
				continue;
			}

			if (currentCell->CheckAllLinked())
			{
				continue;
			}

			std::vector<int> matchIndex;

			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					if (CheckMatch(currentCell->GetPositions()[i], otherCell->GetPositions()[j]))
					{
						matchIndex.emplace_back(i);
						matchIndex.emplace_back(j);
						break;
					}
				}
			}

			if (matchIndex.size() != 4)
			{
				continue;
			}

			currentCell->SetNeighbor(otherCell, matchIndex[0], matchIndex[2]);
		}
	}
}

NavMesh::~NavMesh()
{
}

void NavMesh::Update(const Input& input)
{
}

void NavMesh::Draw()
{

}

const std::array<std::shared_ptr<NavMeshCells>, 3>& NavMesh::GetNeighbors(int index) const
{
	return cells_[index]->GetNeighbors();
}

bool NavMesh::FindPath(NavMeshPath& path, int startID, const VECTOR& startPos, int goalID, const VECTOR& goalPos)
{
	std::shared_ptr<NavNode> currentNode = nullptr;

	// íTçıó\íËÉmÅ[ÉhÇäiî[
	std::vector<std::shared_ptr<NavNode>> openList;

	// íTçıèIóπÉmÅ[ÉhÇäiî[
	std::vector<std::shared_ptr<NavNode>> closeList;

	std::shared_ptr<NavNode> startNode = std::make_shared<NavNode>(goalID);

	startNode->gCost = 0.0f;

	openList.emplace_back(startNode);

	int loopCount = 0;
	const int loopLimit = 1000;

	bool success = false;

	while (openList.size() != 0 && loopCount < loopLimit)
	{
		++loopCount;

		currentNode = PopLowestFCostNode(openList);

		if (currentNode->id == startID)
		{
			success = true;
			break;
		}

		closeList.emplace_back(currentNode);

		auto currentCell = cells_[currentNode->id];

		for (auto neighbor : currentCell->GetNeighbors())
		{
			if (neighbor == nullptr)
			{
				continue;
			}

			auto closeListItr = std::find_if(closeList.begin(), closeList.end(), [neighbor](std::shared_ptr<NavNode> node) 
				{ 
					return node->id == neighbor->GetID(); 
				});

			if (closeListItr != closeList.end())
			{
				continue;
			}

			float totalCost = currentNode->gCost + CalcHeuristic(currentCell->GetCenter(), neighbor->GetCenter());

			std::shared_ptr<NavNode> neighborNode = std::make_shared<NavNode>(neighbor->GetID());

			auto openListItr = std::find_if(openList.begin(), openList.end(), [neighbor](std::shared_ptr<NavNode> node)
				{
					return node->id == neighbor->GetID();
				});

			if (openListItr != openList.end())
			{
				if (totalCost < neighborNode->gCost)
				{
					neighborNode->parent = currentNode;
					neighborNode->gCost = totalCost;
				}
			}
			else
			{
				neighborNode->parent = currentNode;
				neighborNode->gCost = totalCost;
				neighborNode->hCost = CalcHeuristic(neighbor->GetCenter(), goalPos);
				openList.emplace_back(neighborNode);
			}
		}
	}

	if (!success)
	{
		return false;
	}

	path.AddWaypoint(startPos);

	if (currentNode->parent != nullptr)
	{
		currentNode = currentNode->parent;
	}

	while (currentNode->parent != nullptr)
	{
		auto currentCell = cells_[currentNode->id];
		auto parentCell = cells_[currentNode->parent->id];
		path.AddWaypoint(currentCell, parentCell);
		currentNode = currentNode->parent;
	}

	path.AddWaypoint(goalPos);

	return true;
}

MV1_REF_POLYGONLIST NavMesh::GetMeshRef()
{
	return MV1GetReferenceMesh(modelHandle_, -1, true);
}

std::shared_ptr<NavNode> NavMesh::PopLowestFCostNode(std::vector<std::shared_ptr<NavNode>>& list)
{
	std::shared_ptr<NavNode> ret = list[0];
	std::vector<std::shared_ptr<NavNode>>::iterator removeItr = list.begin();
	int minCost = FLT_MAX;

	for (auto itr = list.begin(); itr != list.end(); ++itr)
	{
		auto node = (*itr);
		if (node->gCost == FLT_MAX || node->hCost == FLT_MAX)
		{
			continue;
		}

		int nowCost = node->gCost + node->hCost;
		if (nowCost < minCost)
		{
			minCost = nowCost;

			ret = node;

			removeItr = itr;
		}
	}

	list.erase(removeItr);
	
	return ret;
}

float NavMesh::CalcHeuristic(const VECTOR& v1, const VECTOR& v2) const
{
	VECTOR diff = VSub(v1, v2);
	return ((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z));
}
