#pragma once
#include "Actor.h"
class Stage:
	public Actor
{
public:
	Stage();
	~Stage();

	void Update(const Input& input)override final;
	void Draw()override final;

	/// <summary>
	/// �R���W�������ʑ���p�|���S���z����擾
	/// </summary>
	/// <param name="center"���S���W</param>
	/// <param name="radius">���a</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CollCheckSphere(const VECTOR& center, float radius)const;

	/// <summary>
	/// �R���W�������ʑ���p�|���S���z����擾
	/// </summary>
	/// <param name="pos1"���W1</param>
	/// <param name="pos2"���W2</param>
	/// <param name="radius">���a</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CollCheckCapsule(const VECTOR& pos1, const VECTOR& pos2, float radius)const;
};