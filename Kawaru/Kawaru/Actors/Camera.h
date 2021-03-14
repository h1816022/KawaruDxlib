#pragma once
#include <memory>
#include "Actor.h"
#include "Players/Player.h"

class Stage;

enum class CAMERA_MODE
{
    PlayerFollow,   // プレイヤーを映す
    TargetFollow,   // ターゲットを映す
    Manual          // ターゲットを指定しない
};

struct Angle
{
    float horizontal;
    float vertical;
};

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
    /// 注視点の座標を取得
    /// </summary>
    /// <returns>注視点の座標</returns>
    const VECTOR& GetTargetPos()const;

    /// <summary>
    /// 注視点の基準とするアクターをセット
    /// </summary>
    /// <param name="target">注視点の基準</param>
    void SetTargetActor(std::shared_ptr<Actor> target);

    /// <summary>
    /// プレイヤーのポインタをセット
    /// </summary>
    /// <param name="player">プレイヤー</param>
    void SetPlayer(std::shared_ptr<Player> player);

    /// <summary>
    /// プレイヤーが見えているなら、見えている部分の座標を取得
    /// </summary>
    /// <param name="visiblePos">見えた座標</param>
    /// <returns>true : 見えている</returns>
    bool GetVisiblePlayerPos(VECTOR& visiblePos);

    void ChangeMode(CAMERA_MODE mode);

    bool GetFollowingPlayerFlag();

private:
    /// <summary>
    /// 角度更新
    /// </summary>
    /// <param name="input">入力情報</param>
    void UpdateAngle(const Input& input);

    /// <summary>
    /// 座標(視点)更新
    /// </summary>
    void UpdatePos();

    /// <summary>
    /// 膨らんだ角度を同等の数値に整える
    /// </summary>
    void ClampAngle();

    Angle CalcAngle(const VECTOR& nowVec, const VECTOR& targetVec);

    /// <summary>
    /// カメラアームの更新
    /// </summary>
    /// <param name="rotY">Y回転</param>
    /// <param name="rotZ">Z回転</param>
    void UpdateArmLength(const MATRIX& rotY, const MATRIX& rotZ);

    void LostPlayer();

    void UpdatePlayerFollowMode();

    void UpdateTargetFollowMode();
    
    void UpdateManualMode();

    /// <summary>
    /// プレイヤーが見えているか
    /// </summary>
    /// <returns>true : 見えている</returns>
    bool CanSeePlayer();

    using UpdateFunc = void (Camera::*)();
    UpdateFunc updaterByMode_;

    VECTOR targetPos_;

    Angle angle_;

    std::shared_ptr<Actor> targetActor_;

    std::shared_ptr<Player> player_;

    const Stage& stage_;

    VECTOR setEye_;
    VECTOR setTarget_;
    
    float targetHeightOffset_;

    float armLength_;

    bool followingPlayer_ = false;

    CAMERA_MODE mode_ = CAMERA_MODE::PlayerFollow;

    VECTOR nowShakeOffset_;
    VECTOR targetShakeOffset_;

    int shakeCount_ = 0;
};