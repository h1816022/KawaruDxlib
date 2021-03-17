#pragma once
#include <memory>

class Actor;

class Collision
{
public:
	Collision(std::shared_ptr<Actor> owner);
	~Collision();

	bool CheckHit(const VECTOR& pos1, const VECTOR& pos2, float radius);
	void Draw();

private:
	std::shared_ptr<Actor> owner_;
};