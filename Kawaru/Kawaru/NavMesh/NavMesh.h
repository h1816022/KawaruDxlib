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

// �o�H�m�[�h
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

/// <summary>
/// �i�r���b�V���{��
/// </summary>
class NavMesh :
    public Actor
{
public:
    NavMesh(Scene& scene);
    ~NavMesh();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// �`�揈��
    /// </summary>
    void Draw();

    /// <summary>
    /// �p�X��T��
    /// </summary>
    /// <param name="path">�������p�X���i�[</param>
    /// <param name="type">�o�H�T���̃^�C�v</param>
    /// <param name="startID">�X�^�[�g�̃|���S����ID</param>
    /// <param name="startPos">�X�^�[�g�̍��W</param>
    /// <param name="goalID">�S�[���̃|���S����ID</param>
    /// <param name="goalPos">�S�[���̍��W</param>
    /// <returns>true : �o�H����������</returns>
    bool FindPath(NavMeshPath& path, NAV_TYPE type, int startID, const VECTOR& startPos, int goalID, const VECTOR& goalPos);

    /// <summary>
    /// �i�r���b�V���̃|���S���f�[�^���擾
    /// </summary>
    /// <returns>�i�r���b�V���̃|���S���f�[�^</returns>
    MV1_REF_POLYGONLIST GetMeshRef();

private:
    /// <summary>
    /// F�R�X�g�̍ł��Ⴂ�m�[�h���擾���A���X�g�������
    /// </summary>
    /// <param name="list">�Q�ƃ��X�g</param>
    /// <returns>F�R�X�g�̍ł��Ⴂ�m�[�h</returns>
    std::shared_ptr<NavNode> PopLowestFCostNode(std::vector<std::shared_ptr<NavNode>>& list);

    /// <summary>
    /// �q���[���X�e�B�b�N�l���v�Z���擾
    /// </summary>
    /// <param name="v1">���W1</param>
    /// <param name="v2">���W2</param>
    /// <returns>H�R�X�g</returns>
    float CalcHeuristic(const VECTOR& v1, const VECTOR& v2)const;

    /// <summary>
    /// �אڃZ�����v�Z
    /// </summary>
    void CalcNeighbor();

    /// <summary>
    /// �אڃZ�����Z�b�g
    /// </summary>
    /// <param name="currentCell">�Z��1</param>
    /// <param name="otherCell">�Z��2</param>
    void SetNeighbor(std::shared_ptr<NavMeshCells>& currentCell, std::shared_ptr<NavMeshCells>& otherCell);

    /// <summary>
    /// �w��Z��2�̈�v������W�̃C���f�b�N�X���擾
    /// </summary>
    /// <param name="outIndices">��v�����C���f�b�N�X���i�[</param>
    /// <param name="type">�o�H�T���̃^�C�v</param>
    /// <param name="currentCell">�Z��1</param>
    /// <param name="otherCell">�Z��2</param>
    void FindMatchingIndices(std::vector<int>& outIndices, NAV_TYPE type, const std::shared_ptr<NavMeshCells>& currentCell, const std::shared_ptr<NavMeshCells>& otherCell);

    std::vector<std::shared_ptr<NavMeshCells>> cells_;
};