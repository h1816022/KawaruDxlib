#pragma once
#include <DxLib.h>
#include <memory>
#include <array>

// ���̃Z���Ƌ��L������f�[�^
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

// �ׂ荇���Z���̃f�[�^
// �ڒn�O��̏ꍇ�ƕ����ꍇ�̃f�[�^
struct NeighborsData
{
	std::array<std::shared_ptr<NavMeshCells>, 3> randed;
	std::array<std::shared_ptr<NavMeshCells>, 3> floated;
};

enum class NAV_TYPE;

/// <summary>
/// �O�p�|���S������̃f�[�^
/// </summary>
class NavMeshCells
{
public:
	NavMeshCells(const VECTOR& pos1, const VECTOR& pos2, const VECTOR& pos3, int id);
	~NavMeshCells();

	/// <summary>
	/// �e�_�̍��W���擾
	/// </summary>
	/// <returns>�|���S�����\��������W</returns>
	const std::array<VECTOR, 3>& GetPositions()const;

	/// <summary>
	/// �ׂ̃Z���̃f�[�^���Z�b�g
	/// </summary>
	/// <param name="type">�T���̃^�C�v</param>
	/// <param name="other">����̃Z��</param>
	/// <param name="index1">���L������̃C���f�b�N�X1</param>
	/// <param name="index2">���L������̃C���f�b�N�X2</param>
	void SetNeighbor(NAV_TYPE type, std::shared_ptr<NavMeshCells> other, int index1, int index2);

	/// <summary>
	/// ���ɂ��̃Z�����ׂƂ��ēo�^����Ă��邩
	/// </summary>
	/// <param name="other">���ׂ�Z��</param>
	/// <returns>true : ���łɂ���</returns>
	bool CheckAlreadyLink(std::shared_ptr<NavMeshCells> other)const;

	/// <summary>
	/// ���ɗׂ荇���Z����3�������Ă��邩
	/// </summary>
	/// <returns>true : �������Ă���</returns>
	bool CheckAllLinked()const;
	
	/// <summary>
	/// �Z�����Ƃ�ID���擾
	/// </summary>
	/// <returns>ID</returns>
	int GetID()const;

	/// <summary>
	/// ���S���W���擾
	/// </summary>
	/// <returns></returns>
	const VECTOR& GetCenter()const;

	/// <summary>
	/// �w��ID�̃Z���Ƌ��L���Ă�������擾
	/// </summary>
	/// <param name="type">�o�H�T���̃^�C�v</param>
	/// <param name="toCellID">����̃Z��</param>
	/// <returns>���L������f�[�^</returns>
	const CellEdge& GetArrivalEdge(NAV_TYPE type, int toCellID)const;

	/// <summary>
	/// �ׂ荇���Z���̎擾
	/// </summary>
	/// <param name="type">�o�H�T���̃^�C�v</param>
	/// <returns>�ׂ荇���Z���z��</returns>
	std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(NAV_TYPE type);

	/// <summary>
	/// �ׂ荇���Z���̎擾
	/// </summary>
	/// <param name="type">�o�H�T���̃^�C�v</param>
	/// <returns>�ׂ荇���Z���z��</returns>
	const std::array<std::shared_ptr<NavMeshCells>, 3>& GetNeighbors(NAV_TYPE type)const;

private:
	std::array<VECTOR, 3> positions_;

	std::array<CellEdge, 3> edges_;

	NeighborsData neighbors_;

	const VECTOR center_;

	int id_;
};