#pragma once
#include "Gimmick.h"

/// <summary>
/// ���Ƃ��΂����Ăׂ�񐔂�������
/// </summary>
class Candle :
    public Gimmick
{
public:
    Candle(Scene& scene, float px, float py, float pz);
    ~Candle();

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
    /// <returns>true : ��������������</returns>
    bool Hit(std::shared_ptr<Actor> hitActor)override final;

private:
    int hitSE_;

    int moveCount_ = 0;
};