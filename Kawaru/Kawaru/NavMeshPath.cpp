#include "NavMeshPath.h"
#include "NavMeshCells.h"

NavMeshPath::NavMeshPath()
{
}

NavMeshPath::~NavMeshPath()
{
}

void NavMeshPath::AddWaypoint(const VECTOR& pos)
{
	waypoints_.emplace_back(Waypoint(pos));
}

void NavMeshPath::AddWaypoint(std::shared_ptr<NavMeshCells> cell, std::shared_ptr<NavMeshCells> parent)
{
	waypoints_.emplace_back(cell->GetCenter(), cell->GetID(), cell->GetArrivalEdge(parent->GetID()));
}

const std::vector<Waypoint>& NavMeshPath::GetWaypoints() const
{
	return waypoints_;
}

Waypoint::Waypoint(const VECTOR& pos, int id, const CellEdge& edge):
	pos(pos), id(id), arrivalEdge(edge)
{
}
