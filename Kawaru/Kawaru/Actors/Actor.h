#pragma once
#include <DxLib.h>
#include <string>

class Input;

class Actor
{
public:
	Actor(const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(const wchar_t* modelFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	virtual ~Actor();

	void Init(const float posX, const float posY, const float posZ);

	virtual void Update(const Input& input) = 0;
	virtual void Draw();

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

protected:
	int modelHandle_ = -1;
	
	VECTOR pos_;

	VECTOR scale_;

	float angle_ = 0.0f;
};