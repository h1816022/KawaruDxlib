#pragma once
#include "Actor.h"

class NavMesh;

class Stage:
	public Actor
{
public:
	Stage(Scene& scene, NavMesh& navMesh);
	~Stage();

	void Update(const Input& input)override final;
	void Draw()override final;

	NavMesh& GetNavMesh()const;

private:
	NavMesh& navMesh_;
};