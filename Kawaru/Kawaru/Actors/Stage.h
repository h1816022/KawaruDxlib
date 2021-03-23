#pragma once
#include <vector>
#include <memory>
#include "Actor.h"

// ギミック設置座標のランダム取得の際のタイプ
enum class RAFFILE_TYPE
{
	None,					// ただランダム
	PriorityToDistantThings	// 遠いものを優先
};

// 一辺を表す二つのVertexのインデックス
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

	/// <summary>
	/// ステージ内で最も低い床面のグリッドごとの中心座標をランダム取得
	/// </summary>
	/// <param name="type">取得タイプ</param>
	/// <returns>ランダムなグリッドの中心座標</returns>
	VECTOR GetRandomBottomCenter(RAFFILE_TYPE type = RAFFILE_TYPE::None);

	/// <summary>
	/// ゴールのアクターを取得
	/// </summary>
	/// <returns>ゴールアクター</returns>
	std::shared_ptr<Goal> GetGoal()const;

	/// <summary>
	/// プレイヤーをセット
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(std::shared_ptr<Player> player);

	/// <summary>
	/// ギミックの初期化、設置
	/// </summary>
	void InitGimmicks();

private:
	/// <summary>
	/// 指定インデックスが二つとも被っている(登録済みの辺)でないものなら登録
	/// </summary>
	/// <param name="index1">Vertexのインデックス1</param>
	/// <param name="index2">Vertexのインデックス2</param>
	void SetHypotIndices(int index1, int index2);

	NavMesh& navMesh_;

	int bottomHandle_ = -1;

	// 底面のポリゴンの斜辺を表すインデックスの組み合わせリスト
	std::vector<Indices> hypotIndices_;

	// 底面のグリッドに区切った時の中心座標
	std::vector<VECTOR> bottomCenters_;

	std::shared_ptr<Goal> goal_;

	std::shared_ptr<Player> player_;
};