#include "UniformLv1.h"
#include "PriorityFixedLvl.h"
#include "PriorityDynamicExpand.h"
#include "ScoreTestPriorityDynamicExpand.h"

#include <string>
#include <stdio.h>
#include <vector>

typedef std::vector<std::string> StrVec;

PriorityMonteCarloBase::ScoreMethod ScoreMethodByName(const std::string& s)
{
	if (s == "MAX")
		return PriorityMonteCarloBase::ScoreMethod::SCORE_METHOD_AVE_MAX;
	if(s == "SMAX")
		return PriorityMonteCarloBase::ScoreMethod::SCORE_METHOD_AVE_MAX_SD;
	if (s == "GSMAX")
		return PriorityMonteCarloBase::ScoreMethod::SCORE_METHOD_AVE_MAX_GSD;
	if(s == "AVE")
		return PriorityMonteCarloBase::ScoreMethod::SCORE_METHOD_TOTAL_AVE;
	return PriorityMonteCarloBase::ScoreMethod::SCORE_METHOD_DEFAULT;
}

PlayerBase* CreatePlayer(int sample, const std::string player, StrVec others)
{
	if (player == std::string("simple"))
	{
		return new UniformLv1MonteCarlo(sample);
	}
	if (player.substr(0,2) == std::string("LV"))
	{
		int lvl = atoi(player.substr(2).c_str());
		if (lvl < 1)
			lvl = 1;
		return new PriorityFixedLvlMonteCarlo(sample, lvl);
	}
	if (player.substr(0, 3) == std::string("DYN"))
	{
		int num_to_expand = atoi(player.substr(3).c_str());
		if (num_to_expand < 2)
			num_to_expand = 2;
		PriorityMonteCarloBase::ScoreMethod method =
			ScoreMethodByName((others.size() > 0) ? others[0] : "");
		return new PriorityDynamicExpandMonteCarlo(sample, num_to_expand, method);
	}
	if (player.substr(0, 3) == std::string("STD"))
	{
		int num_to_expand = atoi(player.substr(3).c_str());
		if (num_to_expand < 2)
			num_to_expand = 2;
		return new ScoreTestPriorityDynamicExpandMonteCarlo(sample, num_to_expand);
	}
	return new UniformLv1MonteCarlo(sample);

}

int main(int argc, char** argv)
{
	int sample = 0;
	if (argc > 1)
		sample = std::atoi(argv[1]);
	if (sample < 1)
		sample = 100;
	std::string pname = "";
	if (argc > 2)
		pname = argv[2];
	StrVec others;
	for (int i = 3; i < argc; i++)
		others.push_back(argv[i]);
	PlayerBase* player = CreatePlayer(sample, pname, others);
	int ret = ReversiPlayerStdioClient(player).StartClient();
	delete player;
	return ret;
}
