#pragma once
#include <string>
#include "../Actor.h"

class Camera;
class Stage;

// アニメーションの名前
enum class PLAYER_ANIM_NAME : int
{
    INVALID = -1,   // 無効
    WALK = 0,       // 歩き
    RUN = 1,        // 走り
    JUMP = 2,       // ジャンプ
    FALL = 3,       // 落下
    IDLE = 4        // 待機
};

class Player :
    public Actor
{
public:
    Player(const Camera& camera, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Player();

    void Update(const Input& input)override final;

    void Draw()override final;

private:
    using UpdateFunc = void (Player::*)();
    UpdateFunc updater_;

    void IdleUpdate();
    void RunUpdate();
    void JumpUpdate();

    void Move(const VECTOR& moveVector);

    void UpdateAngle();

    /// <summary>
    /// アニメーションの変更
    /// </summary>
    /// <param name="playAnim">アニメーションの名前</param>
    void ChangeAnim(PLAYER_ANIM_NAME playAnim);

    /// <summary>
    /// アニメーションを更新
    /// </summary>
    void UpdateAnim();

    /// <summary>
    /// アニメーションのブレンド状況を更新
    /// </summary>
    void UpdateAnimBlendRate();

    void DrawShadow();

    float jumpPower_ = 0.0f;

    VECTOR moveDirection_;

    const Camera& camera_;

    const Stage& stage_;

    int state_ = 0;

    int playAnim1_;				// 再生しているアニメーション１のアタッチ番号( -1:何もアニメーションがアタッチされていない )
    float animPlayCount1_;			// 再生しているアニメーション１の再生時間
    int playAnim2_;				// 再生しているアニメーション２のアタッチ番号( -1:何もアニメーションがアタッチされていない )
    float animPlayCount2_;			// 再生しているアニメーション２の再生時間
    float animBlendRate_;				// 再生しているアニメーション１と２のブレンド率

    int shadowHandle_;
};