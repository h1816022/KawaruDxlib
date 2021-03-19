#pragma once
#include "Actor.h"

class NavMesh;

/// <summary>
/// ステージ自体のクラス
/// </summary>
class Stage:
	public Actor
{
public:
	Stage(Scene& scene, NavMesh& navMesh);
	~Stage();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override final;

	/// <summary>
	/// ナビメッシュを取得
	/// </summary>
	/// <returns></returns>
	NavMesh& GetNavMesh()const;

private:
	NavMesh& navMesh_;
};