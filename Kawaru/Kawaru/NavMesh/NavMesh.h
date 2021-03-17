#pragma once
#include <vector>
#include <memory>
#include <array>
#include "../Actors/Actor.h"

class NavNode;
class NavMeshCells;
class NavMeshPath;

// �ǂ������O��Ōo�H�T�����邩
enum class NAV_TYPE
{
    Grounded,   // �ڒn����
    Floated     // ������
};

struct NavNode
{
    NavNode(int id) :
        id(id), gCost(FLT_MAX), hCost(FLT_MAX) {}

    int id;

    // �X�^�[�g�m�[�h���炱�̃m�[�h�܂ł̍ŏ��R�X�g
    float gCost;

    // ���̃m�[�h����S�[���܂ł̐���R�X�g
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