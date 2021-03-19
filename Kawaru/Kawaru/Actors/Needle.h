#pragma once
#include "Gimmick.h"

class GameplayingScene;

/// <summary>
/// �g�Q
/// ��������������
/// </summary>
class Needle :
    public Gimmick
{
public:
    Needle(GameplayingScene& scene);
    ~Needle();

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

private:
    int hitSE_;
};