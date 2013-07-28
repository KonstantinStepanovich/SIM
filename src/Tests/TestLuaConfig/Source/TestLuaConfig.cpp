#include "stdafx.h"

#include "Database/LuaConfig.hpp"
#include "Algorithm/Exception.hpp"

using namespace Sim;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		LuaConfig config("script.lua");
		config.open("tbl1");
		printf("tbl1\n");
		{
			config.open("numkey");
			printf("\tnumkey\n");
			{
				printf("\t\t1 = %d\n", config.get<int>(1));
				printf("\t\t2 = %f\n", config.get<float>(2));
				printf("\t\t3 = %s\n", config.get<std::string>(3).c_str());
				printf("\t\t4 = %s\n", config.get<bool>(4) ? "true" : "false");
				config.open(5);
				{
					printf("\t\t\tt5\n");
					printf("\t\t\tval1 = %d\n", config.get<int>("val1"));
					printf("\t\t\tval2 = %d\n", config.get<int>("val2"));
				}
				config.close();
			}
			config.close();

			config.open("strkey");
			printf("\tstrkey\n");
			{							
				printf("\t\tintval = %d\n", config.get<int>("intval"));
				printf("\t\tfloatval = %f\n", config.get<float>("floatval"));
				printf("\t\tstrval = %s\n", config.get<std::string>("strval").c_str());
				printf("\t\tboolval = %s\n", config.get<bool>("boolval") ? "true" : "false");
				config.open("tblval");
				printf("\t\ttblval\n");
				{
					printf("\t\t\tval1 = %d\n", config.get<int>("val1"));
					printf("\t\t\tval2 = %d\n", config.get<int>("val2"));
				}
				config.close();
			}
			config.close();
		}
		config.close();

		config.open("tbl2");
		printf("tbl2\n");
		{
			printf("\tval1 = %d\n", config.get<int>("val1"));
			printf("\tval2 = %s\n", config.get<std::string>("val2").c_str());
		}
		config.close();

		config.open("tbl3");
		printf("tbl3\n");
		{
			config.start();
			while(config.next())
			{
				int key = config.getKey<int>();
				std::string val = config.getVal<std::string>();
				printf("\t%d = %s\n", key, val.c_str());
			}
			config.end();
		}
		config.close();

		config.open("tbl4");
		printf("tbl4\n");
		{
			config.start();
			while(config.next())
			{
				std::string key = config.getKey<std::string>();
				printf("\t%s\n", key.c_str());
				config.open("val1");
				printf("\t\tval1\n");
					printf("\t\t\t1 = %d\n", config.get<int>(1));
					printf("\t\t\t2 = %d\n", config.get<int>(2));
				config.close();
				
				config.open("val2");
				printf("\t\tval2\n");
					printf("\t\t\t1 = %d\n", config.get<int>(1));
					printf("\t\t\t2 = %d\n", config.get<int>(2));
				config.close();
			}
			config.end();
		}
		config.close();
	}
	catch (Sim::Exception & e)
	{
		printf(e.what());
		return 1;
	}
	printf("OK!");
	return 0;
}