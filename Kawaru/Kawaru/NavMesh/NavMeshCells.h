#pragma once
#include <DxLib.h>
#include <memory>
#include <array>

// 他のセルと共有する線データ
struct CellEdge
{
	CellEdge() :
		left(VECTOR()), right(VECTOR()) {};

	CellEdge(const VECTOR& l, const VECTOR& r) :
		left(l), right(r) {}

	VECTOR left;
	VECTOR right;
};

class NavMeshCells;

// 隣り合うセルのデータ
// 接地前提の場合と浮く場合のデータ
struct NeighborsData
{
	std::array<std::shared_ptr<NavMeshCells>, 3> randed;
	std::array<std::shared_ptr<NavMeshCells>, 3> floated;
};

enum class NAV_TYPE;

/// <summary>
/// 三角ポリゴン一つ分のデータ
/// </summary>
class NavMeshCells
{
public:
	NavMeshCells(const VECTOR& pos1, const VECTOR& pos2, const VECTOR& pos3, int id);
	~NavMeshCells();

	/// <summary>
	/// 各点の座標を取得
	/// </summary>
	/// <returns>ポリゴンを構成する座標</returns>
	const std::array<VECTOR, 3>& GetPositions()const;

	/// <summary>
	/// 隣のセルのデータをセット
	/// </summary>
	/// <param name="type">探索のタイプ</param>
	/// <param name="other">相手のセル</param>
	/// <param name="index1">共有する線のインデックス1</param>
	/// <param name="index2">共有する線のインデックス2</param>
	void SetNeighbor(NAV_TYPE type, std::shared_ptr<NavMeshCells> other, int index1, int index2);

	/// <summary>
	/// 既にこのセルが隣として登録されているか
	/// </summary>
	/// <param name="other">調べるセル</param>
	/// <returns>true : すでにある</returns>
	bool CheckAlreadyLink(std::shared_ptr<NavMeshCells> other)const;

	/// <summary>
	/// 既に隣り合うセルが3つ見つかっているか
	/// </summary>
	/// <returns>true : 見つかっている</returns>
	bool CheckAllLinked()const;
	
	/// <summary>
	/// セルごとのIDを取得
	/// </summary>
	/// <returns>ID</returns>
	int GetID()const;

	/// <summary>
	/// 中心座標を取得
	/// </summary>
	/// <returns></returns>
	const VECTOR& GetCenter()const;

	/// <summary>
	/// 指定IDのセルと共有している線を取得
	/// </summary>
	/// <param name="type">経路探索のタイプ</param>
	/// <param name="toCellID">相手のセル</param>
	/// <returns>共有する線データ</returns>
	const CellEdge& GetArrivalEdge(NAV_TYPE type, int toCellID)const;

	/// <summary>
	/// 隣り合うセルの取得
	/// </summary>
	/// <param name="type">経路探索のタイプ</param>
	/// <returns>隣り合うセル配列</returns>
	std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(NAV_TYPE type);

	/// <summary>
	/// 隣り合うセルの取得
	/// </summary>
	/// <param name="type">経路探索のタイプ</param>
	/// <returns>隣り合うセル配列</returns>
	const std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(NAV_TYPE type)const;

private:
	std::array<VECTOR, 3> positions_;

	std::array<CellEdge, 3> edges_;

	NeighborsData neighbors_;

	const VECTOR center_;

	int id_;
};