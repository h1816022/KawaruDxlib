#pragma once
struct ResultData
{
	ResultData();

	void Reset();

	unsigned int playTime = 0;
	unsigned int callCount = 0;

	bool gameClear = false;
	bool gameOver = false;
};