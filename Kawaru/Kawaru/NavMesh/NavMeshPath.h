#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include "NavMeshCells.h"

// �ړ��o�H�̈�_
struct Waypoint
{
	Waypoint(const VECTOR& pos);

	Waypoint(const VECTOR& pos, int id, const CellEdge& edge);

	VECTOR pos;
	int id;

	CellEdge arrivalEdge;
};

/// <summary>
/// �ړ��o�H
/// </summary>
class NavMeshPath
{
public:
	NavMeshPath();
	~NavMeshPath();

	/// <summary>
	/// �ړ��o�H�̓_��ǉ�
	/// </summary>
	/// <param name="pos">���W</param>
	void AddWaypoint(const VECTOR& pos);

	/// <summary>
	/// �ړ��o�H�̓_��ǉ�
	/// </summary>
	/// <param name="type">�o�H�T���̃^�C�v</param>
	/// <param name="cell">�Y���Z��</param>
	/// <param name="parent">�e�Z��</param>
	void AddWaypoint(NAV_TYPE type, std::shared_ptr<NavMeshCells> cell, std::shared_ptr<NavMeshCells> parent);

	/// <summary>
	/// �ړ��o�H���擾
	/// </summary>
	/// <returns>�ړ��o�H�z��</returns>
	const std::vector<Waypoint>& GetWaypoints()const;

	/// <summary>
	/// �W�O�U�O�Ȍo�H��^�������ɂ��擾
	/// </summary>
	/// <param name="cornerOffsetRatio">�R�[�i�[����̃I�t�Z�b�g�l</param>
	/// <returns></returns>
	std::vector<VECTOR> GetStraightPath(float cornerOffsetRatio)const;

private:
	std::vector<Waypoint> waypoints_;
};