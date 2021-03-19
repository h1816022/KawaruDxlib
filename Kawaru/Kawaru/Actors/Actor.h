#pragma once
#include <DxLib.h>
#include <string>
#include <memory>
#include "../Geometry.h"

class Input;
class Scene;

/// <summary>
/// シーン内で、座標を持ち動く物の総合的な基底クラス
/// </summary>
class Actor
{
public:
	Actor(Scene& scene, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(Scene& scene, const wchar_t* modelFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	virtual ~Actor();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="modelFilePath">表示モデルのファイルのパス</param>
	/// <param name="motionFilePath">アニメーションのファイルのパス</param>
	/// <param name="posX">X座標</param>
	/// <param name="posY">Y座標</param>
	/// <param name="posZ">Z座標</param>
	void Init(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const float posX, const float posY, const float posZ);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	virtual void Update(const Input& input) = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 現在座標の取得
	/// </summary>
	/// <returns>現在座標</returns>
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

	/// <summary>
	/// タグを取得
	/// </summary>
	/// <returns>タグ</returns>
	const std::wstring& GetTag()const;

	/// <summary>
	/// 当たり判定用カプセルの取得
	/// </summary>
	/// <param name="pos">現在座標</param>
	/// <returns>当たり判定用カプセルのデータ</returns>
	virtual Capsule3D GetCollisionCapsule(const VECTOR& pos);

	/// <summary>
	/// このアクターを消す
	/// </summary>
	/// <returns>true : 消せた false : 既に消されているか、消すことができなかった</returns>
	virtual bool Destroy();

	/// <summary>
	/// このアクターが死んでいるか
	/// </summary>
	/// <returns>true : 死んでいる</returns>
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