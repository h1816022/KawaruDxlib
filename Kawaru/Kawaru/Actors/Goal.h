#pragma once
#include <memory>
#include "Gimmick.h"

class Scene;
class Actor;
class Input;

/// <summary>
/// 触れるとクリアとなるアクター
/// </summary>
class Goal :
    public Gimmick
{
    Goal(Scene& scene);
    ~Goal();

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
};

