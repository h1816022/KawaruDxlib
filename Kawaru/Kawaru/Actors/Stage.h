#pragma once
#include "Actor.h"
class Stage:
	public Actor
{
public:
	Stage();
	~Stage();

	void Update(const Input& input)override final;
	void Draw()override final;
};