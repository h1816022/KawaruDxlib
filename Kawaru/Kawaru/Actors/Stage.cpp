#include <random>
#include "Stage.h"
#include "../Scenes/Scene.h"
#include "../Systems/Input.h"
#include "Gimmicks/Needle.h"
#include "Gimmicks/Goal.h"
#include "Gimmicks/Candle.h"
#include "Players/Player.h"

namespace
{
	constexpr int NEEDLE_NUM = 20;

	constexpr int CANDLE_NUM = 10;
}

Stage::Stage(Scene& scene, NavMesh& navMesh):
	Actor(scene, L"Resources/Models/TestMap2.mqo"), navMesh_(navMesh)
{
	bottomHandle_ = MV1LoadModel(L"Resources/Models/TestBottom.mqo");
	MV1SetPosition(bottomHandle_, pos_);

	auto polyData = MV1GetReferenceMesh(bottomHandle_, -1, true);

	for (int i = 0; i < polyData.PolygonNum; ++i)
	{
		const float length01 = GetLengthSQ(VSub(polyData.Vertexs[polyData.Polygons[i].VIndex[0]].Position, polyData.Vertexs[polyData.Polygons[i].VIndex[1]].Position));
		const float length12 = GetLengthSQ(VSub(polyData.Vertexs[polyData.Polygons[i].VIndex[1]].Position, polyData.Vertexs[polyData.Polygons[i].VIndex[2]].Position));
		const float length20 = GetLengthSQ(VSub(polyData.Vertexs[polyData.Polygons[i].VIndex[2]].Position, polyData.Vertexs[polyData.Polygons[i].VIndex[0]].Position));

		if (length01 > length12)
		{
			if (length01 > length20)
			{
				SetHypotIndices(polyData.Polygons[i].VIndex[0], polyData.Polygons[i].VIndex[1]);
			}
			else
			{
				SetHypotIndices(polyData.Polygons[i].VIndex[0], polyData.Polygons[i].VIndex[2]);
			}
		}
		else
		{
			if (length12 > length20)
			{
				SetHypotIndices(polyData.Polygons[i].VIndex[1], polyData.Polygons[i].VIndex[2]);
			}
			else
			{
				SetHypotIndices(polyData.Polygons[i].VIndex[0], polyData.Polygons[i].VIndex[2]);
			}
		}
	}

	for (auto indices : hypotIndices_)
	{
		bottomCenters_.emplace_back(VScale(VAdd(polyData.Vertexs[indices.index1].Position, polyData.Vertexs[indices.index2].Position), 0.5f));
	}
}

void Stage::InitGimmicks()
{
	for (int i = 0; i < NEEDLE_NUM; ++i)
	{
		VECTOR pos = GetRandomBottomCenter();
		scene_.AddActors(std::make_shared<Needle>(scene_, pos.x, pos.y, pos.z));
	}

	for (int i = 0; i < CANDLE_NUM; ++i)
	{
		VECTOR pos = GetRandomBottomCenter();
		scene_.AddActors(std::make_shared<Candle>(scene_, pos.x, pos.y, pos.z));
	}

	VECTOR goalPos = GetRandomBottomCenter(RAFFILE_TYPE::PriorityToDistantThings);
	goal_ = std::make_shared<Goal>(scene_, goalPos.x, goalPos.y, goalPos.z);
	scene_.AddActors(goal_);
}

Stage::~Stage()
{
}

void Stage::Update(const Input& input)
{
}

void Stage::Draw()
{
	MV1DrawModel(modelHandle_);
}

NavMesh& Stage::GetNavMesh() const
{
	return navMesh_;
}

VECTOR Stage::GetRandomBottomCenter(RAFFILE_TYPE type)
{
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::shuffle(bottomCenters_.begin(), bottomCenters_.end(), engine);

	int index = 0;
	VECTOR ret = VECTOR();

	switch (type)
	{
	case RAFFILE_TYPE::None:
		break;

	case RAFFILE_TYPE::PriorityToDistantThings:
		float distance = GetLength2DSQ(VSub(player_->GetPos(), bottomCenters_[0]));

		constexpr int CHOICE_NUM = 10;
		for (int i = 1; i < CHOICE_NUM; ++i)
		{
			float tmpDistance = GetLength2DSQ(VSub(player_->GetPos(), bottomCenters_[i]));
			if (distance < tmpDistance)
			{
				distance = tmpDistance;
				index = i;
			}
		}
		break;
	}

	ret = bottomCenters_[index];
	bottomCenters_.erase(bottomCenters_.begin() + index);

	return ret;
}

std::shared_ptr<Goal> Stage::GetGoal()const
{
	return goal_;
}

void Stage::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

void Stage::SetHypotIndices(int index1, int index2)
{
	for (auto indices : hypotIndices_)
	{
		if ((indices.index1 == index1 && indices.index2 == index2) ||
			(indices.index1 == index2 && indices.index2 == index1))
		{
			return;
		}
	}

	hypotIndices_.emplace_back(index1, index2);
}
