#pragma once
#include <DxLib.h>
#include <string>
#include <memory>
#include "../Geometry.h"

class Input;
class Scene;

/// <summary>
/// �V�[�����ŁA���W�������������̑����I�Ȋ��N���X
/// </summary>
class Actor
{
public:
	Actor(Scene& scene, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(Scene& scene, const wchar_t* modelFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	virtual ~Actor();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="modelFilePath">�\�����f���̃t�@�C���̃p�X</param>
	/// <param name="motionFilePath">�A�j���[�V�����̃t�@�C���̃p�X</param>
	/// <param name="posX">X���W</param>
	/// <param name="posY">Y���W</param>
	/// <param name="posZ">Z���W</param>
	void Init(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX, const float posY, const float posZ);

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	virtual void Update(const Input& input) = 0;

	/// <summary>
	/// �`�揈��
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// ���ݍ��W�̎擾
	/// </summary>
	/// <returns>���ݍ��W</returns>
	const VECTOR& GetPos()const;

	/// <summary>
	/// �����Ǝ����̃��f���̓����蔻��
	/// </summary>
	/// <param name="start">�����̎n�[</param>
	/// <param name="end">�����̏I�[</param>
	DxLib::MV1_COLL_RESULT_POLY CheckHitLine(const VECTOR& start, const VECTOR& end)const;

	/// <summary>
	/// ���Ǝ����̃��f���̓����蔻��
	/// </summary>
	/// <param name="center">���S���W</param>
	/// <param name="radius">���a</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CheckHitSphere(const VECTOR& center, float radius)const;

	/// <summary>
	/// �J�v�Z���Ǝ����̃��f���̓����蔻��
	/// </summary>
	/// <param name="pos1">���W1</param>
	/// <param name="pos2">���W2</param>
	/// <param name="radius">���a</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CheckHitCapsule(const VECTOR& pos1, const VECTOR& pos2, float radius)const;

	/// <summary>
	/// �^�O���擾
	/// </summary>
	/// <returns>�^�O</returns>
	const std::wstring& GetTag()const;

	/// <summary>
	/// �����蔻��p�J�v�Z���̎擾
	/// </summary>
	/// <param name="pos">���ݍ��W</param>
	/// <returns>�����蔻��p�J�v�Z���̃f�[�^</returns>
	virtual Capsule3D GetCollisionCapsule(const VECTOR& pos);

	/// <summary>
	/// ���̃A�N�^�[������
	/// </summary>
	/// <returns>true : ������ false : ���ɏ�����Ă��邩�A�������Ƃ��ł��Ȃ�����</returns>
	virtual bool Destroy();

	/// <summary>
	/// ���̃A�N�^�[������ł��邩
	/// </summary>
	/// <returns>true : ����ł���</returns>
	bool CheckIsDead()const;

protected:
	int modelHandle_ = -1;
	
	VECTOR pos_;

	VECTOR scale_;

	float angle_ = 0.0f;

	std::wstring tag_;

	Scene& scene_;

	bool isDead_ = false;
};