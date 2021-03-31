#pragma once
#include <memory>
#include "Character.h"

class Input;
class Stage;
class NavMeshMoveComponent;
class Camera;

/// <summary>
/// �J�����𑀍삷��S�[�X�g
/// </summary>
class Ghost :
    public Character
{
public:
    Ghost(Scene& scene, Camera& camera, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Ghost();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// �`�揈��
    /// </summary>
    void Draw();

    /// <summary>
    /// �v���C���[�̂��ƂɌĂ�
    /// </summary>
    /// <returns>true : �Ăׂ� false : �ĂׂȂ����A���ɌĂ΂�Č������Ă���</returns>
    bool Call();

    /// <summary>
    /// �Q�[���N���A���̏���
    /// </summary>
    void EndGame();

    void InitPos();

private:
    /// <summary>
    /// �v���C���[�̂��ƂɌ�����
    /// </summary>
    void ApproachPlayer();

    /// <summary>
    /// �w����W�֌�����
    /// </summary>
    /// <param name="target">�ڕW�̍��W</param>
    void ApproachTarget(const VECTOR& target);

    /// <summary>
    /// �w����W���f���̂ɓK�x�ȋ������Ƃ������W���擾
    /// </summary>
    /// <param name="target">�f���������W</param>
    /// <returns></returns>
    VECTOR CalcApproachPos(const VECTOR& target);

    /// <summary>
    /// �ړ��I�����̏���
    /// </summary>
    void EndMove();

    std::unique_ptr<NavMeshMoveComponent> navMeshMoveComponent_;

    float floatingOffset_;

    Camera& camera_;

    bool followingPlayer_ = false;

    int floatingCount_ = 0;

    float moveSpeed_;

    VECTOR oldMoveDirection_;

    bool gameEnd_ = false;

    VECTOR goalPos_;

    int moveSE_ = -1;
};