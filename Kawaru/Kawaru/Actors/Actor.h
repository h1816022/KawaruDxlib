#pragma once
#include <DxLib.h>
#include <string>
#include <memory>
#include "../Geometry.h"

class Input;
class Scene;

class Actor
{
public:
	Actor(Scene& scene, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(Scene& scene, const wchar_t* modelFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	virtual ~Actor();

	void Init(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX, const float posY, const float posZ);

	virtual void Update(const Input& input) = 0;
	virtual void Draw();

	const VECTOR& GetPos()const;

	/// <summary>
	/// 線分と自分のモデルの当たり判定
	/// </summary>
	/// <param name="start">線分の始端</param>
	/// <param name="end">線分の終端</param>
	DxLib::MV1_COLL_RESULT_POLY CheckHitLine(const VECTOR& start, const VECTOR& end)const;

	/// <summary>
	/// 球と自分のモデルの当たり判定
	/// </summary>
	/// <param name="center">中心座標</param>
	/// <param name="radius">半径</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CheckHitSphere(const VECTOR& center, float radius)const;

	/// <summary>
	/// カプセルと自分のモデルの当たり判定
	/// </summary>
	/// <param name="pos1">座標1</param>
	/// <param name="pos2">座標2</param>
	/// <param name="radius">半径</param>
	DxLib::MV1_COLL_RESULT_POLY_DIM CheckHitCapsule(const VECTOR& pos1, const VECTOR& pos2, float radius)const;

	const std::wstring& GetTag()const;

	virtual Capsule3D GetCollisionCapsule(const VECTOR& pos);

	virtual void Destroy();

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