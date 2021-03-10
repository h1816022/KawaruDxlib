#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include "NavMeshCells.h"

struct Waypoint
{
	Waypoint(const VECTOR& pos);

	Waypoint(const VECTOR& pos, int id, const CellEdge& edge);

	VECTOR pos;
	int id;

	CellEdge arrivalEdge;
};

class NavMeshPath
{
public:
	NavMeshPath();
	~NavMeshPath();

	void AddWaypoint(const VECTOR& pos);

	void AddWaypoint(std::shared_ptr<NavMeshCells> cell, std::shared_ptr<NavMeshCells> parent);

	const std::vector<Waypoint>& GetWaypoints()const;

	std::vector<VECTOR> GetStraightPath(float cornerOffsetRatio)const;

private:
	std::vector<Waypoint> waypoints_;
};