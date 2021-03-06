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
	/// コリジョン結果代入用ポリゴン配列を取得
	/// </summary>
	/// <param name="center"中心座標</param>
	/// <param name="radius">半径</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CollCheckSphere(const VECTOR& center, float radius)const;

	/// <summary>
	/// コリジョン結果代入用ポリゴン配列を取得
	/// </summary>
	/// <param name="pos1"座標1</param>
	/// <param name="pos2"座標2</param>
	/// <param name="radius">半径</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CollCheckCapsule(const VECTOR& pos1, const VECTOR& pos2, float radius)const;
};