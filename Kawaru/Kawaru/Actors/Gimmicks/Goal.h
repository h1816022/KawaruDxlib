#pragma once
#include <memory>
#include "Gimmick.h"

class Scene;
class Actor;
class Input;

/// <summary>
/// �G���ƃN���A
/// </summary>
class Goal :
    public Gimmick
{
public:
    Goal(Scene& scene, float px, float py, float pz);
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
    /// <returns>true : ��������������</returns>
    bool Hit(std::shared_ptr<Actor> hitActor)override final;

private:
    int suuSE_ = -1;
    int clearSE_ = -1;
};

