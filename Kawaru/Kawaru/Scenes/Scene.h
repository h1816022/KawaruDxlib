#pragma once
#include <stdint.h>
#include <memory>
#include <string>
#include "../Geometry.h"

class SceneController;
class Input;
class Actor;

enum class FADE_MODE
{
	Non,
	In,
	Out
};

namespace
{
	// フェードにかける時間
	constexpr uint32_t FADE_INTERVAL = 60;

	// 画面枠の大きさ
	constexpr int LETTERBOX_MASK_LENGTH = 50;

	// シーンの表示を重ねるときのマスクの透明度
	constexpr int OVERLAP_TRANSPARENCY = 120;

	// メニューの透明度
	constexpr int MENU_TRANSPARENCY = 200;
}

class Scene
{
	friend SceneController;

public:
	virtual ~Scene();

	virtual void Update(const Input& input) {};
	
	virtual void Draw() {};

	Scene(const Scene& scene) = default;

	/// <summary>
	/// フェードイン終了時の処理
	/// </summary>
	virtual void EndFadeIn() {};

	/// <summary>
	///  フェードアウト終了時の処理
	/// </summary>
	virtual void EndFadeOut() {};

	void AddActors(std::shared_ptr<Actor> actor);

	std::vector<std::shared_ptr<Actor>> GetActors(const std::wstring& tag);

	//void DestroyActor(std::shared_ptr<Actor> actor);

protected:
	Scene(SceneController& controller);

	void UpdateFade(const Input& input);

	/// <summary>
	/// フェードを開始する
	/// </summary>
	/// <param name="mode">イン / アウトを指定</param>
	void StartFade(const FADE_MODE& mode);

	/// <summary>
	/// 様々な描画の後の画面効果(フェードなど)を描画
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 影付きの文字列を描画
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="text">描画する文字列</param>
	/// <param name="color">文字色</param>
	void DrawDropShadowString(const Position2& pos, const wchar_t* text, unsigned int color = 0xffffff);

	void UpdateActors(const Input& input);
	void DrawActors();

	SceneController& controller_;

private:
	/// <summary>
	/// フェード処理
	/// </summary>
	/// <param name="rate">濃度</param>
	void Fade(float rate);

	FADE_MODE fadeMode_;

	int fadeCount_;

	float nowFadeRate = 0.0f;

	std::vector<std::shared_ptr<Actor>> actors_;
};