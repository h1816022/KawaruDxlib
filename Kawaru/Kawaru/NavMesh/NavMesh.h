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

// 経路ノード
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

/// <summary>
/// ナビメッシュ本体
/// </summary>
class NavMesh :
    public Actor
{
public:
    NavMesh(Scene& scene);
    ~NavMesh();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

    /// <summary>
    /// パスを探索
    /// </summary>
    /// <param name="path">見つけたパスを格納</param>
    /// <param name="type">経路探索のタイプ</param>
    /// <param name="startID">スタートのポリゴンのID</param>
    /// <param name="startPos">スタートの座標</param>
    /// <param name="goalID">ゴールのポリゴンのID</param>
    /// <param name="goalPos">ゴールの座標</param>
    /// <returns>true : 経路が見つかった</returns>
    bool FindPath(NavMeshPath& path, NAV_TYPE type, int startID, const VECTOR& startPos, int goalID, const VECTOR& goalPos);

    /// <summary>
    /// ナビメッシュのポリゴンデータを取得
    /// </summary>
    /// <returns>ナビメッシュのポリゴンデータ</returns>
    MV1_REF_POLYGONLIST GetMeshRef();

private:
    /// <summary>
    /// Fコストの最も低いノードを取得し、リストから消す
    /// </summary>
    /// <param name="list">参照リスト</param>
    /// <returns>Fコストの最も低いノード</returns>
    std::shared_ptr<NavNode> PopLowestFCostNode(std::vector<std::shared_ptr<NavNode>>& list);

    /// <summary>
    /// ヒューリスティック値を計算し取得
    /// </summary>
    /// <param name="v1">座標1</param>
    /// <param name="v2">座標2</param>
    /// <returns>Hコスト</returns>
    float CalcHeuristic(const VECTOR& v1, const VECTOR& v2)const;

    /// <summary>
    /// 隣接セルを計算
    /// </summary>
    void CalcNeighbor();

    /// <summary>
    /// 隣接セルをセット
    /// </summary>
    /// <param name="currentCell">セル1</param>
    /// <param name="otherCell">セル2</param>
    void SetNeighbor(std::shared_ptr<NavMeshCells>& currentCell, std::shared_ptr<NavMeshCells>& otherCell);

    /// <summary>
    /// 指定セル2つの一致する座標のインデックスを取得
    /// </summary>
    /// <param name="outIndices">一致したインデックスを格納</param>
    /// <param name="type">経路探索のタイプ</param>
    /// <param name="currentCell">セル1</param>
    /// <param name="otherCell">セル2</param>
    void FindMatchingIndices(std::vector<int>& outIndices, NAV_TYPE type, const std::shared_ptr<NavMeshCells>& currentCell, const std::shared_ptr<NavMeshCells>& otherCell);

    std::vector<std::shared_ptr<NavMeshCells>> cells_;
};