#include	"deasmengine.h"
#include	"OneOpcode.h"
#include	<iostream>
#include	<fstream>
//#include <stdlib.h>


int main(int argc, char* argv[])
{
	OneOpcode oneopc;
	outputinfo *Output=new outputinfo;	
	long TotalLenth = 0;
	std::streamoff templenth=0;
	int mod = 32;
	oneopc.init_size(mod);
		 
	std::ifstream map_file("D:\\test.bin");
	if (!map_file.is_open())
	{
		std::cout << "Error   opening file"; exit(1);
	}

	
	map_file.seekg(0, std::ios::end);
	templenth = map_file.tellg();//获取文件长度
	map_file.seekg(0, std::ios::beg);
	while (1)
	{
		if(templenth <= map_file.tellg())
		break;
		for (int j = 0; j < 16; j++)
			{
			if (!(templenth == map_file.tellg()))
				oneopc.CodeToDeasm[j] = map_file.get();
			else
				oneopc.CodeToDeasm[j] = 0;
			}
		TotalLenth += oneopc.OnOneOpcode(Output,mod);
		map_file.seekg(TotalLenth, std::ios::beg);
		//for (int i = 0; i < Output->length; i++)
		//	std::cout << std::hex << unsigned short(oneopc.CodeToDeasm[i]);
		std::cout << Output->CodeToDeasmmble<<" " << Output->deasmed << " " << Output->length << '\n' << std::endl;
		memset(Output, 0, sizeof(outputinfo));
		oneopc.HasGetModrm = false;
	}
	system("pause");
	free(Output);
	map_file.close();
	return 0;
}

