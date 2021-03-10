#pragma once
#include <vector>
#include <memory>
#include <array>
#include "Actor.h"

class NavNode;
class NavMeshCells;
class NavMeshPath;

struct NavNode
{
    NavNode(int id) :
        id(id), gCost(FLT_MAX), hCost(FLT_MAX) {}

    int id;

    // スタートノードからこのノードまでの最小コスト
    float gCost;

    // このノードからゴールまでの推定コスト
    float hCost;

    std::shared_ptr<NavNode> parent = nullptr;
};

class NavMesh :
    public Actor
{
public:
    NavMesh();
    ~NavMesh();

    void Update(const Input& input)override final;
    void Draw();

    const std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(int index)const;

    bool FindPath(NavMeshPath& path, int startID, const VECTOR& startPos, int goalID, const VECTOR& goalPos);

    MV1_REF_POLYGONLIST GetMeshRef();

private:
    std::shared_ptr<NavNode> PopLowestFCostNode(std::vector<std::shared_ptr<NavNode>>& list);

    float CalcHeuristic(const VECTOR& v1, const VECTOR& v2)const;

    std::vector<std::shared_ptr<NavMeshCells>> cells_;
};