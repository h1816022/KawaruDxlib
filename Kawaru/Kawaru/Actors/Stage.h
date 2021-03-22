#pragma once
#include <vector>
#include <memory>
#include "Actor.h"

enum class RAFFILE_TYPE
{
	None,					// ���������_��
	PriorityToDistantThings	// �������̂�D��
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
/// �X�e�[�W���̂̃N���X
/// </summary>
class Stage:
	public Actor
{
public:
	Stage(Scene& scene, NavMesh& navMesh);
	~Stage();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override final;

	/// <summary>
	/// �i�r���b�V�����擾
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