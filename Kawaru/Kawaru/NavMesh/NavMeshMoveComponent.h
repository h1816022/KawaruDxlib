#pragma once
#include <DxLib.h>
#include <vector>

class Actor;
class Stage;
enum class NAV_TYPE;

/// <summary>
/// アクターからナビメッシュを用いる用コンポーネント
/// </summary>
class NavMeshMoveComponent
{
public:
	NavMeshMoveComponent(const Actor& owner, const Stage& stage, NAV_TYPE type);
	~NavMeshMoveComponent();

	/// <summary>
	/// パスの探索
	/// </summary>
	/// <param name="startPos">開始位置</param>
	/// <param name="goalPos">ゴールの位置</param>
	/// <returns>true : パスが見つかった</returns>
	bool CalcPath(const VECTOR& startPos, const VECTOR& goalPos);

	/// <summary>
	/// パスの取得
	/// </summary>
	/// <returns>パス</returns>
	const std::vector<VECTOR> GetPaths()const;

	/// <summary>
	/// デバッグ用描画処理
	/// </summary>
	void Draw()const;

	/// <summary>
	/// パスが存在するか
	/// </summary>
	/// <returns>true : 存在する</returns>
	bool CheckPathExists()const;

	/// <summary>
	/// 次の目標座標を取得
	/// </summary>
	/// <returns>目標座標(なければ原点)を取得</returns>
	VECTOR GetNextTargetPos();

	/// <summary>
	/// ゴールに到着しているか
	/// </summary>
	/// <param name="radius">到着したとみなす半径</param>
	/// <returns>true : 到着している</returns>
	bool ReachToGoal(float radius)const;

	/// <summary>
	/// パスの更新処理
	/// </summary>
	/// <param name="goalReachradius">ゴールに到着したとみなす半径</param>
	void Update(float goalReachradius);

private:
	const Stage& stage_;

	std::vector<VECTOR> paths_;

	const Actor& owner_;

	const NAV_TYPE type_;
};