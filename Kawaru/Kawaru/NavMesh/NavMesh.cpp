#include "NavMesh.h"
#include "../NavMesh/NavMeshCells.h"
#include "../NavMesh/NavMeshPath.h"
#include "../Tools/Geometry.h"

namespace
{
	// スコア計算時の、y軸上の移動量に掛かる倍率
	constexpr float CLIMB_HEURISTIC_RATE = 10.0f;
}

NavMesh::NavMesh(Scene& scene):
	Actor(scene, L"Resources/Models/TestNavMesh.mqo")
{
	MV1SetupReferenceMesh(modelHandle_, -1, true);
	auto result = MV1GetReferenceMesh(modelHandle_, -1, true);
	
	cells_.reserve(result.PolygonNum);

	for (int index = 0; index < result.PolygonNum; ++index)
	{
		auto poly = result.Polygons[index];
		auto vert = result.Vertexs;
		cells_.emplace_back(std::make_shared<NavMeshCells>(vert[poly.VIndex[0]].Position, vert[poly.VIndex[1]].Position, vert[poly.VIndex[2]].Position, index));
	}

	CalcNeighbor();
}

NavMesh::~NavMesh()
{
}

bool NavMesh::FindPath(NavMeshPath& path, NAV_TYPE type, int startID, const VECTOR& startPos, int goalID, const VECTOR& goalPos)
{
	std::shared_ptr<NavNode> currentNode = nullptr;

	// 探索予定ノードを格納
	std::vector<std::shared_ptr<NavNode>> openList;

	// 探索終了ノードを格納
	std::vector<std::shared_ptr<NavNode>> closeList;

	std::shared_ptr<NavNode> startNode = std::make_shared<NavNode>(goalID);

	startNode->gCost = 0.0f;

	openList.emplace_back(startNode);

	int loopCount = 0;
	constexpr int LOOP_LIMIT = 1000;

	bool success = false;

	while (openList.size() != 0 && loopCount < LOOP_LIMIT)
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

		for (const auto& neighbor : currentCell->GetNeighbors(type))
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
		path.AddWaypoint(type, currentCell, parentCell);
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
	float minCost = FLT_MAX;

	for (auto itr = list.begin(); itr != list.end(); ++itr)
	{
		auto node = (*itr);
		if (node->gCost == FLT_MAX || node->hCost == FLT_MAX)
		{
			continue;
		}

		float nowCost = node->gCost + node->hCost;
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
	return sqrtf((diff.x * diff.x) + (diff.y * diff.y) * CLIMB_HEURISTIC_RATE + (diff.z * diff.z));
}

void NavMesh::CalcNeighbor()
{
	for (auto& currentCell : cells_)
	{
		for (auto& otherCell : cells_)
		{
			// 同じものを比べようとしているなら次へ
			if (currentCell == otherCell)
			{
				continue;
			}

			// 3辺とも隣接セルを見つけているなら次へ
			if (currentCell->CheckAllLinked())
			{
				continue;
			}

			// 既にリンクさせているなら次へ
			if (currentCell->CheckAlreadyLink(otherCell))
			{
				continue;
			}

			SetNeighbor(currentCell, otherCell);
		}
	}
}

void NavMesh::SetNeighbor(std::shared_ptr<NavMeshCells>& currentCell, std::shared_ptr<NavMeshCells>& otherCell)
{
	std::vector<int> matchIndices;

	// 一致する頂点インデックスを、Floatedタイプ(Y軸のズレを無視する)で取得
	FindMatchingIndices(matchIndices, NAV_TYPE::Floated, currentCell, otherCell);

	// 共有する辺が見つかった(取得できたインデックスが4つ)なら
	constexpr int NUM_OF_CAN_DETERMINE_FOUND_SHARED_EDGE = 4;
	if (matchIndices.size() == NUM_OF_CAN_DETERMINE_FOUND_SHARED_EDGE)
	{
		// 空路としては繋がってるとみなし、隣接セルに登録
		currentCell->SetNeighbor(NAV_TYPE::Floated, otherCell, matchIndices[0], matchIndices[2]);
		otherCell->SetNeighbor(NAV_TYPE::Floated, currentCell, matchIndices[1], matchIndices[3]);

		// 更にY軸のズレも見られない場合、陸路としても繋がっているとして登録
		// なお今回は斜めの床など無いため、お互いのセルの一点(インデックス0と1)を比べ差がなければ同一の高さにあるとする
		constexpr float THRESHOLD = 0.1f;
		if (abs(currentCell->GetPositions()[matchIndices[0]].y) - abs(otherCell->GetPositions()[matchIndices[1]].y) < THRESHOLD)
		{
			currentCell->SetNeighbor(NAV_TYPE::Grounded, otherCell, matchIndices[0], matchIndices[2]);
			otherCell->SetNeighbor(NAV_TYPE::Grounded, currentCell, matchIndices[1], matchIndices[3]);
		}
	}
}

void NavMesh::FindMatchingIndices(std::vector<int>& outIndices, NAV_TYPE type, const std::shared_ptr<NavMeshCells>& currentCell, const std::shared_ptr<NavMeshCells>& otherCell)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (type == NAV_TYPE::Grounded)
			{
				if (CheckMatch(currentCell->GetPositions()[i], otherCell->GetPositions()[j]))
				{
					outIndices.emplace_back(i);
					outIndices.emplace_back(j);
				}
			}
			else
			{
				if (CheckMatch2D(currentCell->GetPositions()[i], otherCell->GetPositions()[j]))
				{
					outIndices.emplace_back(i);
					outIndices.emplace_back(j);

				}
			}
		}
	}
}
