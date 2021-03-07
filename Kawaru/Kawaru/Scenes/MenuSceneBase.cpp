#include <DxLib.h>
#include "MenuSceneBase.h"
#include "../Tools/Arithmetic.h"
#include "../Application.h"
#include "../Systems/Input.h"
#include "SceneController.h"

namespace
{
	// 次のメニューを表示した際のオフセット値
	constexpr int NEXT_MENU_OFFSET = 10;

	// タイトルのX座標オフセット
	constexpr int MENU_TITLE_OFFSET_X = 32;

	// 基本のカーソル移動時の傾き持続時間
	constexpr int CURSOR_ROTA_TIME = 8;

	// カーソル移動の速さ
	constexpr int CURSOR_SPEED = 10;

	// 基本のカーソル移動時の傾き量
	constexpr double DEF_CURSOR_ROTA_AMOUNT = 0.5;

	// カーソル移動が端で止まった時の傾き量
	constexpr double EDGE_CURSOR_ROTA_AMOUNT = 0.3;

	// 選ばれたときの追加X軸座標オフセット
	constexpr int SELECT_MENU_ADD_OFFSET_X = 10;
}

MenuSceneBase::MenuSceneBase(SceneController& controller,
	bool blankStart,
	const MenuOffsets& offsets) :
	Scene(controller),
	menuOffsetX_(offsets.menuOffsetX),
	menuIntervalOffsetY_(offsets.menuIntervalOffsetY)
{
	if (blankStart)
	{
		updater_ = &MenuSceneBase::BlankUpdate;
		drawer_ = &MenuSceneBase::BlankDraw;
	}
	else
	{
		updater_ = &MenuSceneBase::NormalUpdate;
		drawer_ = &MenuSceneBase::NormalDraw;
	}

	cursorH_ = LoadGraph(L"Images/Indicator.png");

	GetGraphSize(cursorH_, &cursorSize_.x, &cursorSize_.y);

	menuOffsetY_ = offsets.menuStartOffsetY;

	choiceColor_ = OVERLAP_MENU_COLOR;

	cursorRotaTime_ = static_cast<int>(static_cast<double>(CURSOR_ROTA_TIME) * (static_cast<double>(menuIntervalOffsetY_) / static_cast<double>(DEFAULT_MENU_INTERVAL_OFFSET_Y)));
}

void MenuSceneBase::InitMenuItems(const wchar_t* text, std::function<void(void)> func)
{
	menuItems_.emplace_back(MenuItem(text, Position2(menuOffsetX_, menuOffsetY_), func));

	menuOffsetY_ += menuIntervalOffsetY_;
}

void MenuSceneBase::InitMenu()
{
	const auto& vpSize = Application::Instance().GetViewport().GetSize();
	InitMenu({ vpSize.w / 2, vpSize.h / 2, vpSize.w / 4 * 2, static_cast<int>(static_cast<float>(vpSize.h) / 1.5f) });
}

void MenuSceneBase::InitMenu(const Rect menuFrame)
{
	menuItems_[0].isActive = true;

	const auto& vpSize = Application::Instance().GetViewport().GetSize();
	menuFrame_ = menuFrame;

	auto& indPos = menuItems_[currentSelectNo_].pos;
	targetCursorPos_ = Position2(menuFrame_.GetLeft() + indPos.x - cursorSize_.x / 2, menuFrame_.GetTop() + indPos.y + cursorSize_.y / 2);
	nowCursorPos_ = targetCursorPos_;
	nowCursorRotaAmount_ = DEF_CURSOR_ROTA_AMOUNT * (static_cast<double>(menuIntervalOffsetY_) / static_cast<double>(DEFAULT_MENU_INTERVAL_OFFSET_Y));

	isInitialized = true;
}

void MenuSceneBase::CloseMenu()
{
	auto s = std::dynamic_pointer_cast<MenuSceneBase>(controller_.GetSceneFromTop(2));

	if (s != nullptr)
	{
		s->offset_ += Vector2(NEXT_MENU_OFFSET, NEXT_MENU_OFFSET);
	}

	controller_.PopScene();
}

void MenuSceneBase::MakeNextMenu(MenuSceneBase* newScene)
{
	controller_.PushScene(newScene);
	newScene->offset_ += Vector2(offset_.x + NEXT_MENU_OFFSET, offset_.y + NEXT_MENU_OFFSET);
	this->offset_ -= Vector2(NEXT_MENU_OFFSET, NEXT_MENU_OFFSET);
}

void MenuSceneBase::Update(const Input& input)
{
	if (!isInitialized)
	{
		return;
	}

	(this->*updater_)(input);
}

void MenuSceneBase::Draw()
{
	if (!isInitialized)
	{
		return;
	}

	(this->*drawer_)();
}

void MenuSceneBase::CursorMoveStart(bool isUp)
{
	if (!isInitialized)
	{
		return;
	}

	auto& indPos = menuItems_[currentSelectNo_].pos;
	targetCursorPos_ = Position2(menuFrame_.GetLeft() + indPos.x - cursorSize_.x / 2, menuFrame_.GetTop() + indPos.y + cursorSize_.y / 2);

	if (nowCursorPos_.y == targetCursorPos_.y)
	{
		nowCursorRotaAmount_ = EDGE_CURSOR_ROTA_AMOUNT * (static_cast<double>(menuIntervalOffsetY_) / static_cast<double>(DEFAULT_MENU_INTERVAL_OFFSET_Y));
	}
	else
	{
		nowCursorRotaAmount_ = DEF_CURSOR_ROTA_AMOUNT * (static_cast<double>(menuIntervalOffsetY_) / static_cast<double>(DEFAULT_MENU_INTERVAL_OFFSET_Y));
	}

	cursorCount_ = 1;

	isUp_ = isUp;
}

void MenuSceneBase::UpdateCursor()
{
	if (nowCursorPos_.y == targetCursorPos_.y)
	{
		menuItems_[currentSelectNo_].isActive = true;
		return;
	}

	if (abs(nowCursorPos_.y - targetCursorPos_.y) < CURSOR_SPEED)
	{
		nowCursorPos_.y = targetCursorPos_.y;
		return;
	}

	if (nowCursorPos_.y < targetCursorPos_.y)
	{
		nowCursorPos_.y += CURSOR_SPEED;
	}
	else
	{
		nowCursorPos_.y -= CURSOR_SPEED;
	}
}

void MenuSceneBase::DrawCursor()
{
	float rate = 0.0f;

	if (!IsDivisible(cursorCount_, cursorRotaTime_))
	{
		rate = 1.0f - abs(cursorCount_ % cursorRotaTime_ / (static_cast<float>(cursorRotaTime_) / 2) - 1.0f);
		++cursorCount_;
	}

	DrawRotaGraph(nowCursorPos_.x + offset_.x, nowCursorPos_.y + offset_.y, 1.0, nowCursorRotaAmount_ * rate * (isUp_ ? 1 : -1), cursorH_, true);
}

void MenuSceneBase::DrawFrame()
{
	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	SetDrawBlendMode(DX_BLENDMODE_MULA, OVERLAP_TRANSPARENCY);

	DrawBox(0, 0, vpSize.w, vpSize.h, 0x000000, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, MENU_TRANSPARENCY);

	DrawBox(menuFrame_.GetLeft() + offset_.x,
		menuFrame_.GetTop() + offset_.y,
		menuFrame_.GetRight() + offset_.x,
		menuFrame_.GetBottom() + offset_.y, 0x000000, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawBox(menuFrame_.GetLeft() + offset_.x,
		menuFrame_.GetTop() + offset_.y,
		menuFrame_.GetRight() + offset_.x,
		menuFrame_.GetBottom() + offset_.y, 0xffffff, false);
}

void MenuSceneBase::DrawMenu(bool isBlank)
{
	// メニュータイトルの表示
	int titleW = GetDrawStringWidth(title_.c_str(), static_cast<int>(std::wcslen(title_.c_str())));
	DrawDropShadowString({ menuFrame_.GetLeft() + (menuFrame_.GetWidth() / 2 - titleW / 2) + offset_.x, menuFrame_.GetTop() + MENU_TITLE_OFFSET_X + offset_.y }, title_.c_str());

	// メニュー項目の表示
	for (auto& menu : menuItems_)
	{
		if (menu.isActive && !isBlank)
		{
			DrawString(menuFrame_.GetLeft() + menu.pos.x + SELECT_MENU_ADD_OFFSET_X + offset_.x, menuFrame_.GetTop() + menu.pos.y + offset_.y, menu.menuText.c_str(), choiceColor_);
		}
		else
		{
			DrawString(menuFrame_.GetLeft() + menu.pos.x + offset_.x, menuFrame_.GetTop() + menu.pos.y + offset_.y, menu.menuText.c_str(), DEFAULT_MENU_COLOR);
		}
	}
}

void MenuSceneBase::BlankUpdate(const Input& input)
{
	if (input.IsTriggered("Up") || input.IsTriggered("Down") || input.IsTriggered("OK"))
	{
		updater_ = &MenuSceneBase::NormalUpdate;
		drawer_ = &MenuSceneBase::NormalDraw;
	}
}

void MenuSceneBase::NormalUpdate(const Input& input)
{
	if (canInput_)
	{
		if (input.IsTriggered("Pause") || input.IsTriggered("Exit"))
		{
			CloseMenu();
			return;
		}

		if (menuItems_[currentSelectNo_].isActive)
		{
			if (input.IsTriggered("OK"))
			{
				menuItems_[currentSelectNo_].func();
				return;
			}

			if (input.IsTriggered("MenuUp"))
			{
				menuItems_[currentSelectNo_].isActive = false;
				currentSelectNo_ = max(currentSelectNo_ - 1, 0);

				CursorMoveStart(true);
			}

			if (input.IsTriggered("MenuDown"))
			{
				menuItems_[currentSelectNo_].isActive = false;
				currentSelectNo_ = min(currentSelectNo_ + 1, (static_cast<int>(menuItems_.size()) - 1));

				CursorMoveStart(false);
			}
		}
	}

	UpdateCursor();
}

void MenuSceneBase::BlankDraw()
{
	DrawFrame();
	DrawMenu(true);
}

void MenuSceneBase::NormalDraw()
{
	DrawFrame();
	DrawMenu(false);
	DrawCursor();
}
