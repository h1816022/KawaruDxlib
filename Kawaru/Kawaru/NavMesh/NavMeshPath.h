#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include "NavMeshCells.h"

// 移動経路の一点
struct Waypoint
{
	Waypoint(const VECTOR& pos);

	Waypoint(const VECTOR& pos, int id, const CellEdge& edge);

	VECTOR pos;
	int id;

	CellEdge arrivalEdge;
};

/// <summary>
/// 移動経路
/// </summary>
class NavMeshPath
{
public:
	NavMeshPath();
	~NavMeshPath();

	/// <summary>
	/// 移動経路の点を追加
	/// </summary>
	/// <param name="pos">座標</param>
	void AddWaypoint(const VECTOR& pos);

	/// <summary>
	/// 移動経路の点を追加
	/// </summary>
	/// <param name="type">経路探索のタイプ</param>
	/// <param name="cell">該当セル</param>
	/// <param name="parent">親セル</param>
	void AddWaypoint(NAV_TYPE type, std::shared_ptr<NavMeshCells> cell, std::shared_ptr<NavMeshCells> parent);

	/// <summary>
	/// 移動経路を取得
	/// </summary>
	/// <returns>移動経路配列</returns>
	const std::vector<Waypoint>& GetWaypoints()const;

	/// <summary>
	/// ジグザグな経路を真っ直ぐにし取得
	/// </summary>
	/// <param name="cornerOffsetRatio">コーナーからのオフセット値</param>
	/// <returns></returns>
	std::vector<VECTOR> GetStraightPath(float cornerOffsetRatio)const;

private:
	std::vector<Waypoint> waypoints_;
};