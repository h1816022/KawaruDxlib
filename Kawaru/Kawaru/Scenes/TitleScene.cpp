#include <DxLib.h>
#include <stdint.h>
#include "TitleScene.h"
#include "SceneController.h"
#include "GameplayingScene.h"
#include "../Systems/Input.h"
#include "../Application.h"
#include "../Geometry.h"
//#include "../Systems/FileManager.h"
//#include "../Systems/File.h"

namespace
{
	// �J�ڂ����悤�Ƃ��Ă���t�F�[�h���n�܂�܂ł̒x��鎞��
	constexpr uint32_t FADE_DELAY = 40;

	// ��{�̓_�ł̃C���^�[�o��
	constexpr uint32_t BLINK_INTERVAL_DEFAULT = 120;

	// ��������Ƃ��̓_�ł̃C���^�[�o��
	constexpr uint32_t BLINK_INTERVAL_FAST = 10;

	// �_�łŏ����Ă���_���܂ł̎���
	constexpr uint32_t BLINK_TIME = 5;

	constexpr uint32_t EMISSIVE_TIME = 30;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller)
{
	updater_ = &TitleScene::NormalUpdate;
	drawer_ = &TitleScene::FadeInDraw;

	count_ = 0;

	StartFade(FadeMode::In);
}

void TitleScene::NormalUpdate(const Input& input)
{
	if (input.IsTriggered("OK"))
	{
		controller_.ChangeScene(new GameplayingScene(controller_));
		return;
	}
}

void TitleScene::FadeinUpdate(const Input&)
{
}

void TitleScene::FadeoutUpdate(const Input&)
{
}

void TitleScene::NormalDraw()
{
	DrawString(0, 0, L"Title", 0xffffffff);
}

void TitleScene::FadeInDraw()
{

}

void TitleScene::FadeOutDraw()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(const Input& input)
{
	count_++;
	(this->*updater_)(input);
	Scene::UpdateFade(input);
}

void TitleScene::Draw()
{
	(this->*drawer_)();

	Scene::PostDraw();
}

void TitleScene::EndFadeIn()
{
	updater_ = &TitleScene::NormalUpdate;
	drawer_ = &TitleScene::NormalDraw;
}

void TitleScene::EndFadeOut()
{
	controller_.ChangeScene(new GameplayingScene(controller_));
}
