#include "ResultData.h"

ResultData::ResultData()
{
}

void ResultData::Reset()
{
	playTime = 0;
	callCount = 0;

	gameClear = false;
	gameOver = false;
}
