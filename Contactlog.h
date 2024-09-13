#pragma once
#include <vector>

class Contactlog
{
public:
	//履歴の追加
	void Add(uint32_t number);

	//履歴のチェック
	bool LogCheck(uint32_t number);

	//履歴抹消
	void Clear();

private:
	//履歴
	std::vector<uint32_t> log_;


};
