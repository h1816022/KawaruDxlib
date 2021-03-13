#pragma once
#include <memory>
#include "Actor.h"
#include "Players/Player.h"

class Stage;

class Camera :
    public Actor
{
public:
    Camera(const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Camera();

    void Update(const Input& input)override final;

    void Draw()override final;

    void SetPos(const VECTOR& pos);

    /// <summary>
    /// �����_�̍��W���擾
    /// </summary>
    /// <returns>�����_�̍��W</returns>
    const VECTOR& GetTargetPos()const;

    /// <summary>
    /// �����_�̊�Ƃ���A�N�^�[���Z�b�g
    /// </summary>
    /// <param name="target">�����_�̊</param>
    void SetTargetActor(std::shared_ptr<Actor> target);

    /// <summary>
    /// �v���C���[�̃|�C���^���Z�b�g
    /// </summary>
    /// <param name="player">�v���C���[</param>
    void SetPlayer(std::shared_ptr<Player> player);

    /// <summary>
    /// �v���C���[�������Ă��邩
    /// </summary>
    /// <returns>true : �����Ă���</returns>
    bool CanSeePlayer();

    /// <summary>
    /// �v���C���[�������Ă��邩
    /// </summary>
    /// <param name="visiblePos">���������W</param>
    /// <returns>true : �����Ă���</returns>
    bool CanSeePlayer(VECTOR& visiblePos);

private:
    /// <summary>
    /// �p�x�X�V
    /// </summary>
    /// <param name="input">���͏��</param>
    void UpdateAngle(const Input& input);

    /// <summary>
    /// ���W(���_)�X�V
    /// </summary>
    void UpdatePos();

    /// <summary>
    /// �c��񂾊p�x�𓯓��̐��l�ɐ�����
    /// </summary>
    void ClampAngle();

    /// <summary>
    /// �J�����A�[���̍X�V
    /// </summary>
    /// <param name="rotY">Y��]</param>
    /// <param name="rotZ">Z��]</param>
    void UpdateArmLength(const MATRIX& rotY, const MATRIX& rotZ);

    VECTOR targetPos_;

    // �����p�x
    float angleH_;

    // �����p�x
    float angleV_;

    std::shared_ptr<Actor> targetActor_;

    std::shared_ptr<Player> player_;

    const Stage& stage_;

    VECTOR setEye_;
    VECTOR setTarget_;
    
    float targetHeightOffset_;

    float armLength_;
    
    float a;
    float b;

    bool c = false;
};