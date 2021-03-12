#pragma once
#include <DxLib.h>
#include <memory>
#include <array>

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

struct NeighborsData
{
	std::array<std::shared_ptr<NavMeshCells>, 3> randed;
	std::array<std::shared_ptr<NavMeshCells>, 3> floated;
};

enum class NAV_TYPE;

class NavMeshCells
{
public:
	NavMeshCells(const VECTOR& pos1, const VECTOR& pos2, const VECTOR& pos3, int id);
	~NavMeshCells();

	const std::array<VECTOR, 3>& GetPositions()const;

	void SetNeighbor(NAV_TYPE type, std::shared_ptr<NavMeshCells> other, int index1, int index2);

	bool CheckAlreadyLink(NAV_TYPE type, std::shared_ptr<NavMeshCells> other)const;

	bool CheckAllLinked(NAV_TYPE type)const;

	int GetID()const;

	const VECTOR& GetCenter()const;

	const CellEdge& GetArrivalEdge(NAV_TYPE type, int toCellID)const;

	std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(NAV_TYPE type);
	const std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(NAV_TYPE type)const;

private:
	std::array<VECTOR, 3> positions_;

	std::array<CellEdge, 3> edges_;

	NeighborsData neighbors_;

	const VECTOR center_;

	int id_;
};