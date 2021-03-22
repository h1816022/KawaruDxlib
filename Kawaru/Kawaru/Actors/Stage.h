#pragma once
#include <vector>
#include <memory>
#include "Actor.h"

enum class RAFFILE_TYPE
{
	None,					// ただランダム
	PriorityToDistantThings	// 遠いものを優先
};

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

	VECTOR GetRandomBottomCenter(RAFFILE_TYPE type = RAFFILE_TYPE::None);

	std::shared_ptr<Goal> GetGoal()const;

	void SetPlayer(std::shared_ptr<Player> player);

	void InitGimmicks();

private:
	void SetIndices(int index1, int index2);

	NavMesh& navMesh_;

	int bottomHandle_ = -1;

	std::vector<Indices> hypotIndices_;

	std::vector<VECTOR> bottomCenters_;

	std::shared_ptr<Goal> goal_;

	std::shared_ptr<Player> player_;
};