#pragma once
#include <memory>
#include "Gimmick.h"

class Scene;
class Actor;
class Input;

/// <summary>
/// 触れるとクリア
/// </summary>
class Goal :
    public Gimmick
{
public:
    Goal(Scene& scene, float px, float py, float pz);
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
    /// <returns>true : 処理を完遂した</returns>
    bool Hit(std::shared_ptr<Actor> hitActor)override final;

private:
    int suuSE_ = -1;
    int clearSE_ = -1;
};

