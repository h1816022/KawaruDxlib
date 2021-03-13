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
    /// プレイヤーが見えているか
    /// </summary>
    /// <returns>true : 見えている</returns>
    bool CanSeePlayer();

    /// <summary>
    /// プレイヤーが見えているか
    /// </summary>
    /// <param name="visiblePos">見えた座標</param>
    /// <returns>true : 見えている</returns>
    bool CanSeePlayer(VECTOR& visiblePos);

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

    /// <summary>
    /// カメラアームの更新
    /// </summary>
    /// <param name="rotY">Y回転</param>
    /// <param name="rotZ">Z回転</param>
    void UpdateArmLength(const MATRIX& rotY, const MATRIX& rotZ);

    VECTOR targetPos_;

    // 水平角度
    float angleH_;

    // 垂直角度
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