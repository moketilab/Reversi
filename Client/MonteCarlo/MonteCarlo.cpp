#include "UniformLv1.h"
#include "PriorityFixedLvl.h"
#include "PriorityDynamicExpand.h"
#include "ScoreTestPriorityDynamicExpand.h"

#include <string>
#include <stdio.h>

PlayerBase* CreatePlayer(int sample, const std::string player)
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
		return new PriorityDynamicExpandMonteCarlo(sample, num_to_expand);
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
	PlayerBase* player = CreatePlayer(sample, pname);
	int ret = ReversiPlayerStdioClient(player).StartClient();
	delete player;
	return ret;
}
