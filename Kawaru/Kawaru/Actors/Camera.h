#pragma once
#include <memory>
#include "Actor.h"
#include "Players/Player.h"

class Stage;

// カメラの動きのモード
enum class CAMERA_MODE
{
    PlayerFollow,   // プレイヤーを映す
    TargetFollow,   // ターゲットを映す
    GameEnd         // 1プレイ終了時
};

// 角度情報
struct Angle
{
    float horizontal;   // 水平角度
    float vertical;     // 垂直角度
};

/// <summary>
/// カメラ関連のクラス
/// </summary>
class Camera :
    public Actor
{
public:
    Camera(Scene& scene, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Camera();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// カメラのブレ感を更新
    /// </summary>
    void UpdateCameraShake();

    /// <summary>
    /// 注視点の座標を取得
    /// </summary>
    /// <returns>注視点の座標</returns>
    const VECTOR& GetTargetPos()const;

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

    /// <summary>
    /// モードの変更
    /// </summary>
    /// <param name="mode">変更するモード</param>
    void ChangeMode(CAMERA_MODE mode);

    /// <summary>
    /// プレイヤーを追従しているか
    /// </summary>
    /// <returns>true : 追従している</returns>
    bool GetFollowingPlayerFlag();

    /// <summary>
    /// 1ゲームが終わった時の処理
    /// </summary>
    void EndGame();

private:
    /// <summary>
    /// 座標(視点)更新
    /// </summary>
    void UpdatePos();

    /// <summary>
    /// プレイヤー追従モードのときの更新処理
    /// </summary>
    void UpdatePlayerFollowMode();
    
    /// <summary>
    /// ゲームクリアしたときの更新処理
    /// </summary>
    void UpdateGameClear();

    /// <summary>
    /// プレイヤーが見えているか
    /// </summary>
    /// <returns>true : 見えている</returns>
    bool CanSeePlayer();

    using UpdateFunc = void (Camera::*)();
    UpdateFunc updaterByMode_;

    // 注視点
    VECTOR targetPos_;

    // カメラの角度
    Angle angle_;

    std::shared_ptr<Player> player_;

    const Stage& stage_;

    // 暫定の視点座標
    VECTOR setEye_;

    // 暫定の注視点座標
    VECTOR setTarget_;
    
    /// <summary>
    /// 注視点の算出時、targetActorの座標から、Y軸方向に足すオフセット値
    /// </summary>
    float targetHeightOffset_;

    /// <summary>
    /// カメラアームの長さ
    /// </summary>
    float armLength_;
    
    // プレイヤーを追従しているか
    bool followingPlayer_ = false;

    CAMERA_MODE mode_ = CAMERA_MODE::PlayerFollow;

    // 現在の手ブレのオフセット値
    VECTOR nowShakeOffset_;

    // 目標の手ブレのオフセット値
    VECTOR targetShakeOffset_;

    // 手ブレ計算用カウント
    int shakeCount_ = 0;
};