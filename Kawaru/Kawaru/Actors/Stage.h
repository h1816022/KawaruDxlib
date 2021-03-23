#pragma once
#include <vector>
#include <memory>
#include "Actor.h"

// �M�~�b�N�ݒu���W�̃����_���擾�̍ۂ̃^�C�v
enum class RAFFILE_TYPE
{
	None,					// ���������_��
	PriorityToDistantThings	// �������̂�D��
};

// ��ӂ�\�����Vertex�̃C���f�b�N�X
struct Indices
{
	Indices(int i1, int i2) :
		index1(i1), index2(i2) {}

	int index1;
	int index2;
};

class NavMesh;
class Goal;
class Player;

/// <summary>
/// �X�e�[�W���̂̃N���X
/// </summary>
class Stage:
	public Actor
{
public:
	Stage(Scene& scene, NavMesh& navMesh);
	~Stage();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override final;

	/// <summary>
	/// �i�r���b�V�����擾
	/// </summary>
	/// <returns></returns>
	NavMesh& GetNavMesh()const;

	/// <summary>
	/// �X�e�[�W���ōł��Ⴂ���ʂ̃O���b�h���Ƃ̒��S���W�������_���擾
	/// </summary>
	/// <param name="type">�擾�^�C�v</param>
	/// <returns>�����_���ȃO���b�h�̒��S���W</returns>
	VECTOR GetRandomBottomCenter(RAFFILE_TYPE type = RAFFILE_TYPE::None);

	/// <summary>
	/// �S�[���̃A�N�^�[���擾
	/// </summary>
	/// <returns>�S�[���A�N�^�[</returns>
	std::shared_ptr<Goal> GetGoal()const;

	/// <summary>
	/// �v���C���[���Z�b�g
	/// </summary>
	/// <param name="player">�v���C���[</param>
	void SetPlayer(std::shared_ptr<Player> player);

	/// <summary>
	/// �M�~�b�N�̏������A�ݒu
	/// </summary>
	void InitGimmicks();

private:
	/// <summary>
	/// �w��C���f�b�N�X����Ƃ�����Ă���(�o�^�ς݂̕�)�łȂ����̂Ȃ�o�^
	/// </summary>
	/// <param name="index1">Vertex�̃C���f�b�N�X1</param>
	/// <param name="index2">Vertex�̃C���f�b�N�X2</param>
	void SetHypotIndices(int index1, int index2);

	NavMesh& navMesh_;

	int bottomHandle_ = -1;

	// ��ʂ̃|���S���̎Εӂ�\���C���f�b�N�X�̑g�ݍ��킹���X�g
	std::vector<Indices> hypotIndices_;

	// ��ʂ̃O���b�h�ɋ�؂������̒��S���W
	std::vector<VECTOR> bottomCenters_;

	std::shared_ptr<Goal> goal_;

	std::shared_ptr<Player> player_;
};