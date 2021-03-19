#pragma once
#include <vector>
#include <functional>
#include "Scene.h"

// メニュー項目ごとのデータ
struct MenuItem
{
	bool isActive = false;
	std::wstring menuText;
	Position2 pos;

	// 実行する処理
	std::function<void(void)> func;

	MenuItem(const wchar_t* str,
		const Position2& p,
		std::function<void(void)> f)
		:menuText(str),
		func(f),
		pos(p)
	{}
};

namespace
{
	// 特に設定のない時の、選択中でないメニュー項目のX軸座標オフセット
	constexpr int DEFAULT_MENU_OFFSET_X = 64;

	// 特に設定のない時の、メニュー項目の最初のY軸座標オフセット
	constexpr int DEFAULT_MENU_START_OFFSET_Y = 96;

	// 特に設定のない時の、メニュー項目の間のY軸座標オフセット
	constexpr int DEFAULT_MENU_INTERVAL_OFFSET_Y = 48;

	// メニュー項目の基本色
	constexpr unsigned int DEFAULT_MENU_COLOR = 0xcccccc;

	// カーソルが重なっているときのメニュー項目の色
	constexpr unsigned int OVERLAP_MENU_COLOR = 0xffff77;

	// 選択されたメニュー項目の色
	constexpr unsigned int CHOICE_MENU_COLOR = 0xff3333;
}

// メニュー項目の表示オフセット
struct MenuOffsets
{
	MenuOffsets(const int x = DEFAULT_MENU_OFFSET_X, const int startY = DEFAULT_MENU_START_OFFSET_Y, const int intervalY = DEFAULT_MENU_INTERVAL_OFFSET_Y) :
		menuOffsetX(x),
		menuStartOffsetY(startY),
		menuIntervalOffsetY(intervalY)
	{};

	// 選択中でないメニュー項目のX軸座標オフセット
	const int menuOffsetX;

	// メニュー項目の最初のY軸座標オフセット
	const int menuStartOffsetY;

	// メニュー項目の間のY軸座標オフセット
	const int menuIntervalOffsetY;
};

/// <summary>
/// メニュー項目を表示させるシーンの基底クラス
/// </summary>
class MenuSceneBase :
	public Scene
{
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void Update(const Input& input)override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;

protected:
	/// <param name="controller">シーンのコントローラー</param>
	/// <param name="blankStart">最初にカーソルを隠して、選んで決定するまでにひと操作必要にするか</param>
	/// <param name="offsets">メニュー項目の色々なオフセット値</param>
	MenuSceneBase(SceneController& controller,
		bool blankStart = false,
		const MenuOffsets& offsets = MenuOffsets()
	);

	/// <summary>
	/// メニュー項目を設定
	/// </summary>
	/// <param name="text">表示テキスト</param>
	/// <param name="func">選択したときの処理</param>
	void InitMenuItems(const wchar_t* text, std::function<void(void)> func);

	/// <summary>
	/// 設定したメニュー項目をもとにカーソルなどの初期設定をする
	/// </summary>
	void InitMenu();

	/// <summary>
	/// 設定したメニュー項目をもとにカーソルなどの初期設定をする
	/// </summary>
	/// <param name="menuFrame">メニューの枠</param>
	void InitMenu(const Rect menuFrame);

	/// <summary>
	/// 呼び出し元のシーンへ戻る
	/// </summary>
	virtual void CloseMenu();

	/// <summary>
	/// 次のメニューシーンを生成
	/// </summary>
	/// <param name="newScene">次のメニューシーン</param>
	void MakeNextMenu(MenuSceneBase* newScene);

	// 項目
	std::vector<MenuItem> menuItems_;

	// 現在選ばれている項目番号
	int currentSelectNo_ = 0;

	// メニュータイトル
	std::wstring title_ = L"タイトル未入力";

	// 入力可能か
	bool canInput_ = true;

	// メニュー表示のズレ量
	Vector2 offset_ = { 0, 0 };

	// 選ばれたメニュー項目の色
	unsigned int choiceColor_;

private:
	/// <summary>
	/// カーソルを動き始めさせる
	/// </summary>
	/// <param name="isUp">true : 上方向, false ; 下方向</param>
	void CursorMoveStart(bool isUp);

	/// <summary>
	/// カーソルの動きを更新
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// カーソルの描画
	/// </summary>
	void DrawCursor();

	/// <summary>
	/// 枠の描画
	/// </summary>
	void DrawFrame();

	/// <summary>
	/// メニュー項目の描画
	/// </summary>
	void DrawMenu(bool isBlank);

	/// <summary>
	/// ひと入力待っているときの更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void BlankUpdate(const Input& input);

	/// <summary>
	/// 通常の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void NormalUpdate(const Input& input);

	using Update_t = void (MenuSceneBase::*)(const Input& input);
	Update_t updater_;

	/// <summary>
	/// ひと入力待っているときの描画処理
	/// </summary>
	void BlankDraw();

	/// <summary>
	/// 通常の描画処理
	/// </summary>
	void NormalDraw();

	using Draw_t = void (MenuSceneBase::*)();
	Draw_t drawer_;

	Vector2 cursorSize_;

	int cursorH_;

	int32_t cursorCount_;

	int cursorRotaTime_;

	// カーソルが上向きに移動している
	bool isUp_ = false;

	// 初期化が完了している
	bool isInitialized = false;

	// 現在のカーソルの座標
	Position2 nowCursorPos_;

	// カーソルの目標地点の座標
	Position2 targetCursorPos_;

	// 現在のカーソルの回転量
	double nowCursorRotaAmount_;

	// メニューの枠
	Rect menuFrame_;

	// メニュー項目のXオフセット値
	int menuOffsetX_ = 0;

	// メニュー項目のYオフセット値
	int menuOffsetY_ = 0;

	// メニュー項目間のオフセット値
	int menuIntervalOffsetY_;
};