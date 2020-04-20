// FarmbeatsapiTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once
#include <iostream>

#include "..\Farmbeatsapi\Farmbeatsapi.h"
#include <libloaderapi.h>
#include "..\Farmbeatsapi\include\rapidjson\document.h"
#include "..\Farmbeatsapi\include\rapidjson\writer.h"
#include "..\Farmbeatsapi\include\rapidjson\stringbuffer.h"
using namespace std;



typedef  IFarmbeatsapi* (WINAPI* Getfarmbeatsapi)();
typedef void  (WINAPI* DestoryFarmbeatsapi)(IFarmbeatsapi* hfarmbeatsapi);



string fbhub_url = "https://xxxxxxxx.azurewebsites.net";
string clientID = "a925xxxx-xxxx-xxxx-xxxx-xxxxf9b4xxxx";
string clientSecret = "xxxxxxxxxxxxxxxxxxx";
string tenantID = "xxxx0275-xxxx-xxxx-xxxx-xxxx0dafxxxx";


string farmid = "xxxxcafc-xxxx-xxxx-xxxx-xxxx08c5xxxx";

IFarmbeatsapi* farmbeatsapi = NULL;


int ConventionalTest();
int GetFarmListTest();
int UploadFileTest();
int main()
{
	//填写一下四个参数


	HMODULE hinst = LoadLibraryA("Farmbeatsapi.dll");

	if (NULL == hinst)
	{

		//资源加载失败!
		cout << "LoadLibraryA Failed! " << "GetLastError =" << GetLastError() << endl;
		system("pause");
		return 0;

	}
	Getfarmbeatsapi GetfarmbeatsapiFuction = (Getfarmbeatsapi)GetProcAddress(hinst, "GetFarmbeatsapi");

	if (NULL == GetfarmbeatsapiFuction)
	{

		//资源加载失败!
		cout << "GetProcAddress Failed! " << endl;
		FreeLibrary(hinst);
		system("pause");
		return 0;

	}
	DestoryFarmbeatsapi DestoryFarmbeatsapiFuction = (DestoryFarmbeatsapi)GetProcAddress(hinst, "DestoryFarmbeatsapi");

	if (NULL == DestoryFarmbeatsapiFuction)
	{

		//资源加载失败!
		cout << "DestoryFarmbeatsapiFuction Failed! " << endl;
		FreeLibrary(hinst);
		system("pause");
		return 0;

	}
	farmbeatsapi = GetfarmbeatsapiFuction();

	//version
	cout << "version = " << farmbeatsapi->FBGetAPIVersion() << endl;


	//login
	int ret = farmbeatsapi->Login(fbhub_url, clientID, clientSecret, tenantID);
	if (ret != FarmbeatsError_Success)
	{
		cout << "Login = " << ret << "************************************************" << endl;
	}
	cout << "Login success!" << endl;

	while (true)
	{
		cout << "Menu" << endl;
		cout << "1.Conventional testing" << endl;
		cout << "2.Get Farm list testing(refresh)" << endl;
		cout << "3.Upload files testing" << endl;
		cout << "0.Exit" << endl;
		string inputstr;
		cin >> inputstr;
		switch (inputstr[0])
		{
		case 0x31:
			ConventionalTest();
			break;
		case 0x32:
			GetFarmListTest();
			break;
		case 0x33:
			UploadFileTest();
			break;
		case 0x30:
			DestoryFarmbeatsapiFuction(farmbeatsapi);
			FreeLibrary(hinst);
			return 0;
		default:

			continue;

			break;
		}

	}




	return 0;
}

int ConventionalTest()
{
	

	//get farm list
	string farmlist;
	int ret = farmbeatsapi->GetFarmList(farmlist);
	if (ret != FarmbeatsError_Success)
	{
		cout << "GetFarmList = " << ret << "************************************************" << endl;
	}
	cout << "GetFarmList success!" << endl;
	cout << farmlist << endl;




	//creat mission
	string MissionID;
	ret = farmbeatsapi->CreatMission(farmid, MissionID);
	if (ret != FarmbeatsError_Success)
	{
		cout << "CreatMission = " << ret << endl;
	}
	cout << "CreatMission success!" << endl;
	cout << MissionID << endl;

	//uploadfile
	//test.tif md5
	//696b85dfcbb475daf0139c67c5cabfad
	ret = farmbeatsapi->UploadFile(MissionID, "test.tif", ".\\test.tif", Image_TIFF, string("696b85dfcbb475daf0139c67c5cabfad"));

	//ret = farmbeatsapi->UploadFile(MissionID, "test.jpg", ".\\test.jpg", Image_JPG);
	//ret = farmbeatsapi->UploadFile(MissionID, "test.tif", ".\\test.tif", Image_PNG);
	//ret = farmbeatsapi->UploadFile(MissionID, "test.png", ".\\test.png", Image_PNG);
	//
	//ret = farmbeatsapi->UploadFile(MissionID, "bigfile.png", ".\\bigfile.png", Image_PNG);
	/*
	不填写MD5则不校验
	ret = farmbeatsapi->UploadFile(MissionID, "test.tif", ".\\test.tif");
	*/
	if (ret != FarmbeatsError_Success)
	{
		cout << "UploadFile = " << ret << endl;
	}
	return 0;
}

int GetFarmListTest()
{
	//get farm list
	string farmlist;
	int ret = farmbeatsapi->GetFarmList(farmlist);
	if (ret != FarmbeatsError_Success)
	{
		cout << "GetFarmList = " << ret << "************************************************" << endl;
		return 0;
	}
	cout << "GetFarmList success!" << endl;
	//cout << farmlist << endl;


	rapidjson::Document doc4Result;
	//(doc4Result.Parse<rapidjson::kParseNumbersAsStringsFlag>(JsonResult.c_str()).HasParseError()
	if (doc4Result.Parse(farmlist.c_str()).HasParseError())
	{
		return FarmbeatsError_AuthenticationError;
	}

	rapidjson::Value& items = doc4Result["Farms"];
	if (items.Size() == 0)
	{
		cout << "no farm found!" << endl;

	}
	else
	{
		cout << "{" << endl;
		for (auto& item : items.GetArray())
		{
			//name
			if (item.HasMember("name"))
			{

				cout << item["name"].GetString() << endl;
			}
		}
		cout << "}" << endl;
	}

	return 0;
}
int UploadFileTest()
{
	

	DWORD start_time;
	DWORD end_time;
	for (int i = 1; i <= 48; i++)
	{
		char szfilename[100] = { 0 };

		char  cdecade = 0x30 + i / 10;
		char cUnit = 0x30 + i % 10;
		sprintf_s(szfilename, "100_0001_00%c%c.JPG", cdecade, cUnit);
		string fileName = szfilename;
		cout << "filename = " << fileName << endl;
		start_time = GetTickCount();
		//creat mission
		string MissionID;
		string testfarmid = "53cae085-b1b5-476e-8d18-05960b783328";
		int ret = farmbeatsapi->CreatMission(testfarmid, MissionID);
		if (ret != FarmbeatsError_Success)
		{
			cout << "CreatMission = " << ret << endl;
		}

		end_time = GetTickCount();

		cout << "CreatMission time is:" << (end_time - start_time) << "ms!" << endl;
		std::cout << std::endl;

		cout << "CreatMission success!" << endl;

		cout << MissionID << endl;
		std::cout << std::endl;
		start_time = GetTickCount();
		ret = farmbeatsapi->UploadFile(MissionID, fileName, ".\\testimage\\" + fileName, Image_JPG, string("696b85dfcbb475daf0139c67c5cabfad"));

		//ret = farmbeatsapi->UploadFile(MissionID, "test.jpg", ".\\test.jpg", Image_JPG);
		//ret = farmbeatsapi->UploadFile(MissionID, "test.tif", ".\\test.tif", Image_PNG);
		//ret = farmbeatsapi->UploadFile(MissionID, "test.png", ".\\test.png", Image_PNG);
		//
		//ret = farmbeatsapi->UploadFile(MissionID, "bigfile.png", ".\\bigfile.png", Image_PNG);
		/*
		不填写MD5则不校验
		ret = farmbeatsapi->UploadFile(MissionID, "test.tif", ".\\test.tif");
		*/
		if (ret != FarmbeatsError_Success)
		{
			cout << "UploadFile = " << ret << endl;
		}
		end_time = GetTickCount();

		cout << "Total UploadFile time is:" << (end_time - start_time) << "ms!" << endl;
		std::cout << std::endl;
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
