#pragma once
#include "Farmbeatsapi.h"
#include <map>
#include <list>
class FarmbeatsapiImpl
{
public:
	FarmbeatsapiImpl();
	~FarmbeatsapiImpl();
	int Login(std::string fbhub_url, std::string clientID, std::string clientSecret, std::string tenantID, std::string& Token);
	int GetFarmList(std::string fbhub_url, std::string Token, std::string& farmlistJson);
	int CheckExtendedTypeExists(std::string fbhub_url, std::string Token, std::string extendedTypeKey, std::string extendedTypeValue, bool& extendedTypeExists, std::string& extendedTypeID, std::string& extendTypeBody);
	int AddExtenedType(std::string fbhub_url, std::string Token, std::string extendedTypeID, std::string extendTypeBody);
	int CreatMission(std::string fbhub_url, std::string Token, std::string farmID, std::string& MissionID);
	int UploadFile(std::string storageURL, std::string Token, std::string filename, std::string filepath, std::string fileMD5, Farmbeats_ImageType imageType);
	int GetSceneFileURL(std::string fbhub_url, std::string Token, std::string MissionID, std::string filename, std::string filepath ,std::string & storageURL, std::string& fileID, Farmbeats_ImageType imageType);
	int DeleteSceneFile(std::string fbhub_url, std::string Token, std::string fileID);
	int ImageConvert2PNG(std::string sourceFile, std::string destFile);
	int ImageCompression(std::string sourceFile, std::string& destFile);
private:
	
	int ParseResult_Login(std::string JsonResult, std::string& Token);
	int ParseResult_GetFarmList(std::string JsonResult, std::string& farmlistJson);
	int ParseReslt_CheckExtendedTypeExists(std::string JsonResult, std::string extendedTypeKey, std::string extendedTypeValue, bool& extendedTypeExists, std::string& extendedTypeID, std::string& extendTypeBody);
	int ParseResult_CreatMission(std::string JsonResult, std::string& MissionID);
	int ParseResult_GetSceneFileURL(std::string JsonResult, std::string& storageURL, std::string& fileID);
	int httpRequest(std::string requestUrl, std::string requstName, std::string httpVerb, std::string  targetResource, std::string requestHeader, std::string requestBody, std::string & responseContent);
	int httpRequest(std::string requestUrl, std::string requstName, std::string httpVerb, std::string  targetResource, std::list<std::string> requestHeaders, std::string requestBody, std::string& responseContent);
	int httpRequest_Uploadfile(std::string requestUrl, std::string requstName, std::string httpVerb, std::string  targetResource, std::list<std::string> requestHeaders, std::string requestBody, std::string& responseContent);
	long fileSize(std::string sourceFile);
	std::wstring string2wstring(std::string str);
	std::string wstring2string(std::wstring wstr);
	std::string GetTimeString();
	
};

