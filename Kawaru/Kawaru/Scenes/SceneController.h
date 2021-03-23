
#pragma once
#include <memory>
#include <deque>
#include <functional>
#include <vector>

class Scene;
class Input;
class Application;

/// <summary>
/// �V�[���̊Ǘ��N���X
/// </summary>
class SceneController
{
	friend Application;

public:
	/// <summary>
	/// ���݂̈�ԏ��(�X�V��������Ă���)�V�[�����I�����A�V�����V�[���𐶐�
	/// </summary>
	/// <param name="newScene">�V�����V�[��</param>
	void ChangeScene(Scene* newScene);

	/// <summary>
	/// ���݂̃V�[�����L�[�v�����܂ܐV�����V�[����ǉ�
	/// </summary>
	/// <param name="newScene">�V�����V�[��</param>
	void PushScene(Scene* newScene);

	/// <summary>
	/// ���݂̈�ԏ��(�X�V��������Ă���)�V�[������w�萔�����I��
	/// </summary>
	void PopScene(unsigned int num = 1);

	/// <summary>
	/// ���݂̃V�[����S�ďI�����A�V�����V�[���𐶐�
	/// </summary>
	/// <param name="newScene">�V�����V�[��</param>
	void CleanChangeScene(Scene* newScene);

	/// <summary>
	/// ���݂̃V�[�������ׂďI��
	/// </summary>
	void ClearScenes();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void Update(const Input& input);

	/// <summary>
	/// �`����
	/// </summary>
	void Draw();

	/// <summary>
	/// ���ݏォ��w��Ԗڂɂ���V�[�����擾
	/// </summary>
	/// <returns>�w��Ԗڂ̃V�[��</returns>
	std::shared_ptr<Scene> GetSceneFromTop(int num);

private:
	SceneController();

	std::deque<std::shared_ptr<Scene>> scenes_;

	std::vector<std::function<void(void)>> postDrawExecuter_;
};