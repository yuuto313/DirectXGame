#include "Contactlog.h"

#include <algorithm>

void Contactlog::Add(uint32_t number)
{
	//履歴に登録
	log_.push_back(number);
}

bool Contactlog::LogCheck(uint32_t number)
{
	return std::any_of(log_.begin(), log_.end(), [number](uint32_t log) { return log == number; });
}

void Contactlog::Clear()
{
	//履歴を消去
	log_.clear();
}
