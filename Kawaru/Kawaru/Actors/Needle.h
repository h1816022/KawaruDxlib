#pragma once
#include "Gimmick.h"

class GameplayingScene;

/// <summary>
/// トゲ
/// 当たったらやられる
/// </summary>
class Needle :
    public Gimmick
{
public:
    Needle(GameplayingScene& scene);
    ~Needle();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw()override final;

protected:
    /// <summary>
    /// 当たり判定を検知したときの処理
    /// </summary>
    /// <param name="hitActor">当たったアクター</param>
    void Hit(std::shared_ptr<Actor> hitActor)override final;

private:
    int hitSE_;
};