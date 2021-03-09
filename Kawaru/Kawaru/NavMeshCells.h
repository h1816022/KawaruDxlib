#pragma once
#include <DxLib.h>
#include <memory>
#include <array>

struct CellEdge
{
	CellEdge() {};

	CellEdge(const VECTOR& l, const VECTOR& r) :
		left(l), right(r) {}

	VECTOR left;
	VECTOR right;
};

class NavMeshCells
{
public:
	NavMeshCells(const VECTOR& pos1, const VECTOR& pos2, const VECTOR& pos3, int id);
	~NavMeshCells();

	const std::array<VECTOR, 3>& GetPositions()const;

	void SetNeighbor(std::shared_ptr<NavMeshCells> other, int index1, int index2);

	bool CheckAlreadyLink(std::shared_ptr<NavMeshCells> other)const;

	bool CheckAllLinked()const;

	int GetID()const;

	const VECTOR& GetCenter()const;

	const std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors()const;

	const CellEdge& GetArrivalEdge(int toCellID)const;

private:
	std::array<VECTOR, 3> positions_;

	std::array<CellEdge, 3> edges_;

	std::array<std::shared_ptr<NavMeshCells>, 3> neighbors_;

	const VECTOR center_;

	int id_;
};