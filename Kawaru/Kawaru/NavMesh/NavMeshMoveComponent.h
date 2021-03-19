#pragma once
#include <DxLib.h>
#include <vector>

class Actor;
class Stage;
enum class NAV_TYPE;

/// <summary>
/// �A�N�^�[����i�r���b�V����p����p�R���|�[�l���g
/// </summary>
class NavMeshMoveComponent
{
public:
	NavMeshMoveComponent(const Actor& owner, const Stage& stage, NAV_TYPE type);
	~NavMeshMoveComponent();

	/// <summary>
	/// �p�X�̒T��
	/// </summary>
	/// <param name="startPos">�J�n�ʒu</param>
	/// <param name="goalPos">�S�[���̈ʒu</param>
	/// <returns>true : �p�X����������</returns>
	bool CalcPath(const VECTOR& startPos, const VECTOR& goalPos);

	/// <summary>
	/// �p�X�̎擾
	/// </summary>
	/// <returns>�p�X</returns>
	const std::vector<VECTOR> GetPaths()const;

	/// <summary>
	/// �f�o�b�O�p�`�揈��
	/// </summary>
	void Draw()const;

	/// <summary>
	/// �p�X�����݂��邩
	/// </summary>
	/// <returns>true : ���݂���</returns>
	bool CheckPathExists()const;

	/// <summary>
	/// ���̖ڕW���W���擾
	/// </summary>
	/// <returns>�ڕW���W(�Ȃ���Ό��_)���擾</returns>
	VECTOR GetNextTargetPos();

	/// <summary>
	/// �S�[���ɓ������Ă��邩
	/// </summary>
	/// <param name="radius">���������Ƃ݂Ȃ����a</param>
	/// <returns>true : �������Ă���</returns>
	bool ReachToGoal(float radius)const;

	/// <summary>
	/// �p�X�̍X�V����
	/// </summary>
	/// <param name="goalReachradius">�S�[���ɓ��������Ƃ݂Ȃ����a</param>
	void Update(float goalReachradius);

private:
	const Stage& stage_;

	std::vector<VECTOR> paths_;

	const Actor& owner_;

	const NAV_TYPE type_;
};