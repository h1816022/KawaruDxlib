#pragma once
#include <string>
#include <vector>
#include "../Character.h"

class Camera;
class Ghost;

/// <summary>
/// ����L�����N�^�[
/// </summary>
class Player :
    public Character
{
public:
    Player(Scene& scene, Camera& camera, Ghost& ghost, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Player();

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
    /// �v���C���[�������邩����p�́A�ڕW���W���X�g�̃I�t�Z�b�g���擾
    /// </summary>
    /// <returns>�ڕW���W���X�g�̃I�t�Z�b�g</returns>
    const std::vector<VECTOR>& GetLineTraceSamplingOffsets()const;

    /// <summary>
    /// ���S����
    /// </summary>
    /// <returns>true : ���S���� false : ���Ɏ��S���Ă��邩�A�|���Ȃ��󋵂ɂ���</returns>
    bool Destroy()override final;

    /// <summary>
    /// 1�Q�[���I��������̏���
    /// </summary>
    void EndGame();

    /// <summary>
    /// ���΂����Ăׂ�񐔂𑝂₷
    /// </summary>
    void AddCallNum();

    /// <summary>
    /// ���΂����Ăׂ�񐔂��擾
    /// </summary>
    /// <returns></returns>
    int GetCallNum()const;

private:
    /// <summary>
    /// �ҋ@��Ԃ̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void IdleUpdate(const Input& input)override final;

    /// <summary>
    /// �ړ���Ԃ̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void RunUpdate(const Input& input)override final;

    /// <summary>
    /// �W�����v��Ԃ̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void JumpUpdate(const Input& input)override final;

    /// <summary>
    /// ���S��Ԃ̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void DestroyUpdate(const Input& input)override final;

    /// <summary>
    /// ����1.0f�̈ړ��x�N�g�����v�Z
    /// </summary>
    /// <param name="upMoveVec">�O�����x�N�g��</param>
    /// <param name="leftMoveVec">�������x�N�g��</param>
    void CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec);

    /// <summary>
    /// �ړ��x�N�g�����v�Z�����
    /// </summary>
    /// <param name="moveVec">�ړ��x�N�g��</param>
    /// <param name="upMoveVec">�O�����x�N�g��</param>
    /// <param name="leftMoveVec">�������x�N�g��</param>
    /// <param name="input">���͏��</param>
    /// <returns>true : �ړ�����</returns>
    bool CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input);

    /// <summary>
    /// �ړ��p�f�[�^�̍X�V
    /// </summary>
    /// <param name="input">���͏��</param>
    void UpdateMove(const Input& input);

    /// <summary>
    /// ���΂����Ă�
    /// </summary>
    /// <returns>true : �Ăׂ� false : �ĂׂȂ��������A���ɌĂ�ł��Č������Ă���</returns>
    bool CallGhost();

    Camera& camera_;
    Ghost& ghost_;

    // �g�[���X���s���ۂɏI�[�Ƃ���T���v�����O���W�́A���g�̍��W����̃I�t�Z�b�g�l
    std::vector<VECTOR> lineTraceSamplingOffsets_;

    int fingerSnapSE_ = -1;

    int callNum_;
};