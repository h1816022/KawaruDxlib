#pragma once
#include "Actor.h"

class NavMesh;

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

private:
	NavMesh& navMesh_;
};