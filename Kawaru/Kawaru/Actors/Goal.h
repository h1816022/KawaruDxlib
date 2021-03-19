#pragma once
#include <memory>
#include "Gimmick.h"

class Scene;
class Actor;
class Input;

/// <summary>
/// �G���ƃN���A�ƂȂ�A�N�^�[
/// </summary>
class Goal :
    public Gimmick
{
    Goal(Scene& scene);
    ~Goal();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void Update(const Input& input)override final;
    
    /// <summary>
    /// �`�揈��
    /// </summary>
    void Draw()override final;

protected:
    /// <summary>
    /// �����蔻������m�����Ƃ��̏���
    /// </summary>
    /// <param name="hitActor">���������A�N�^�[</param>
    void Hit(std::shared_ptr<Actor> hitActor)override final;
};

