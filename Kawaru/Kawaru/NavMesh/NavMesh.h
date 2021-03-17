#pragma once
#include <vector>
#include <memory>
#include <array>
#include "../Actors/Actor.h"

class NavNode;
class NavMeshCells;
class NavMeshPath;

// どういう前提で経路探索するか
enum class NAV_TYPE
{
    Grounded,   // 接地する
    Floated     // 浮かぶ
};

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
    NavMesh(Scene& scene);
    ~NavMesh();

    void Update(const Input& input)override final;
    void Draw();

    bool FindPath(NavMeshPath& path, NAV_TYPE type, int startID, const VECTOR& startPos, int goalID, const VECTOR& goalPos);

    MV1_REF_POLYGONLIST GetMeshRef();

private:
    std::shared_ptr<NavNode> PopLowestFCostNode(std::vector<std::shared_ptr<NavNode>>& list);

    float CalcHeuristic(const VECTOR& v1, const VECTOR& v2)const;

    void CalcNeighbor(NAV_TYPE type);

    void FindMatchingIndices(std::vector<int>& outIndices, NAV_TYPE type, const std::shared_ptr<NavMeshCells>& currentCell, const std::shared_ptr<NavMeshCells>& otherCell);

    std::vector<std::shared_ptr<NavMeshCells>> cells_;
};