#pragma once
#include <DxLib.h>
#include <string>

class Input;

class Actor
{
public:
	Actor(const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	Actor(const wchar_t* modelFilePath, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
	virtual ~Actor();

	void Init(const float posX, const float posY, const float posZ);

	virtual void Update(const Input& input) = 0;
	virtual void Draw();

	const VECTOR& GetPos()const;

protected:
	int modelHandle_ = -1;
	
	VECTOR pos_;

	VECTOR scale_;

	float angle_ = 0.0f;
};