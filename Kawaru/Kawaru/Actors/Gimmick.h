#pragma once
#include <memory>
#include "Actor.h"

class Scene;

/// <summary>
/// �w��^�O�̃A�N�^�[�Ƃ̓����蔻������A�N�^�[���N���X
/// </summary>
class Gimmick :
    public Actor
{
public:
    Gimmick(Scene& scene, const wchar_t* modelFilePath, const wchar_t* targetTag);
    virtual ~Gimmick();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    virtual void Update(const Input& input)override;

    /// <summary>
    /// �`�揈��
    /// </summary>
    virtual void Draw() = 0;

protected:
    /// <summary>
    /// �����蔻������m�������̏���
    /// </summary>
    /// <param name="hitActor">���������A�N�^�[</param>
    virtual void Hit(std::shared_ptr<Actor> hitActor) = 0;

    const std::wstring targetTag_;

    Scene& scene_;
};

