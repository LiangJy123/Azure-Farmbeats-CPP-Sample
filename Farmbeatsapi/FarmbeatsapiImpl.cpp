#include "pch.h"
#include <winhttp.h>
#include "FarmbeatsapiImpl.h"
#include "include/opencv2/opencv.hpp"
#pragma comment(lib,"Winhttp.lib")
#pragma comment(lib,"opencv_world345.lib")
FarmbeatsapiImpl::FarmbeatsapiImpl()
{

}
FarmbeatsapiImpl::~FarmbeatsapiImpl()
{

}


int FarmbeatsapiImpl::Login(std::string fbhub_url, std::string clientID, std::string clientSecret, std::string tenantID, std::string& Token)
{
	std::string JsonResult;

	int ret = httpRequest(
		std::string("login.microsoftonline.com"),
		std::string("AAD Service API"),
		std::string("POST"),
		std::string("/" + tenantID + "/oauth2/token?api-version=1.0"),
		std::string("Content-Type: application/x-www-form-urlencoded"),
		std::string("grant_type=client_credentials&client_id=" + clientID + "&client_secret=" + clientSecret + "&resource=" + fbhub_url),
		JsonResult
	);

	if (ret != FarmbeatsError_Success)
	{
		return ret;
	}

	return ParseResult_Login(JsonResult, Token);

}
int FarmbeatsapiImpl::GetFarmList(std::string fbhub_url, std::string Token, std::string& farmlistJson)
{
	std::string JsonResult;
	int ret = httpRequest(
		fbhub_url,
		std::string("Farmbeats API GetFarmlist"),
		std::string("GET"),
		std::string("/Farm?maxItems=1500"),
		std::string("Authorization: " + Token),
		std::string(""),
		JsonResult
	);
	if (ret != FarmbeatsError_Success)
	{
		return ret;
	}
	return ParseResult_GetFarmList(JsonResult, farmlistJson);

}
int FarmbeatsapiImpl::CheckExtendedTypeExists(std::string fbhub_url, std::string Token, std::string extendedTypeKey, std::string extendedTypeValue, bool& extendedTypeExists, std::string& extendedTypeID, std::string& extendTypeBody)
{
	std::string JsonResult;
	int ret = httpRequest(
		fbhub_url,
		std::string("Farmbeats API CheckExtendedType"),
		std::string("GET"),
		std::string("/ExtendedType?keys=" + extendedTypeKey + "&maxItems=1500"),
		std::string("Authorization: " + Token),
		std::string(""),
		JsonResult
	);
	if (ret != FarmbeatsError_Success)
	{
		return ret;
	}

	return ParseReslt_CheckExtendedTypeExists(JsonResult, extendedTypeKey, extendedTypeValue, extendedTypeExists, extendedTypeID, extendTypeBody);

}
int FarmbeatsapiImpl::AddExtenedType(std::string fbhub_url, std::string Token, std::string extendedTypeID, std::string extendTypeBody)
{
	std::string JsonResult;
	std::list<std::string> tmp_stringlist;
	tmp_stringlist.push_back("Authorization: " + Token);
	tmp_stringlist.push_back("Content-Type: application/json");
	int ret = httpRequest(
		fbhub_url,
		std::string("Farmbeats API AddExtenedType"),
		std::string("PUT"),
		std::string("/ExtendedType/" + extendedTypeID),
		tmp_stringlist,
		std::string(extendTypeBody),
		JsonResult
	);

	return ret;
}
int FarmbeatsapiImpl::CreatMission(std::string fbhub_url, std::string Token, std::string farmID, std::string& MissionID)
{
	std::string JsonResult;
	std::list<std::string> tmp_stringlist;
	tmp_stringlist.push_back("Authorization: " + Token);
	tmp_stringlist.push_back("Content-Type: application/json");
	std::string timestr = GetTimeString();
	std::string httpbody("{\
		\"type\": \"drone\",\
		\"source\" : \"DJI\",\
		\"farmId\" : \"" + farmID + "\",\
		\"date\" : \"" + timestr + "\",\
		\"sequence\" : 0,\
		\"name\" : \"Mission_" + timestr + "\",\
		\"description\" : \"Mission files upload\",\
		\"properties\" : {}}");
	int ret = httpRequest(
		fbhub_url,
		std::string("Farmbeats API CreatMission"),
		std::string("POST"),
		std::string("/Scene"),
		tmp_stringlist,
		httpbody,
		JsonResult
	);
	if (ret != FarmbeatsError_Success)
	{
		return ret;
	}
	return ParseResult_CreatMission(JsonResult, MissionID);
}
int FarmbeatsapiImpl::GetSceneFileURL(std::string fbhub_url, std::string Token, std::string MissionID, std::string filename, std::string filepath, std::string& storageURL, std::string& fileID,Farmbeats_ImageType imageType )
{
	std::string imageTypeMIME = imageType == Image_TIFF ? "image/tiff" : "image/png";
	std::string JsonResult;
	std::list<std::string> tmp_stringlist;
	tmp_stringlist.push_back("Authorization: " + Token);
	tmp_stringlist.push_back("Content-Type: application/json");
	std::string timestr = GetTimeString();
	std::string httpbody("{\
		\"sceneId\": \"" + MissionID + "\",\
		\"type\" : \"orthomosaic\",\
		\"contentType\" : \""+ imageTypeMIME +"\",\
		\"name\" : \"" + filename + "\",\
		\"description\" : \" Mission file upload\",\
		\"properties\" : {}}");
	int ret = httpRequest(
		fbhub_url,
		std::string("Farmbeats API GetSceneFileURL"),
		std::string("POST"),
		std::string("/SceneFile"),
		tmp_stringlist,
		httpbody,
		JsonResult
	);
	if (ret != FarmbeatsError_Success)
	{
		return ret;
	}
	return ParseResult_GetSceneFileURL(JsonResult, storageURL, fileID);
}
int FarmbeatsapiImpl::DeleteSceneFile(std::string fbhub_url, std::string Token, std::string fileID)
{
	std::string JsonResult;


	int ret = httpRequest(
		fbhub_url,
		std::string("Farmbeats API GetSceneFileURL"),
		std::string("POST"),
		std::string("/SceneFile"),
		std::string("Authorization: " + Token),
		std::string(""),
		JsonResult
	);

	return ret;

}
int FarmbeatsapiImpl::UploadFile(std::string storageURL, std::string Token, std::string filename, std::string filepath, std::string fileMD5, Farmbeats_ImageType imageType)
{

	std::string imageTypeMIME = imageType == Image_TIFF ? "image/tiff" : "image/png";

	if (imageType == Image_JPG)
	{
		//JPG=>PNG,so that can't support MD5 check.
		fileMD5.clear();
	}
	std::string JsonResult;

	std::list<std::string> tmp_stringlist;
	tmp_stringlist.push_back("x-ms-blob-type: BlockBlob");
	tmp_stringlist.push_back("Content-Type: "+ imageTypeMIME);

	//Content-MD5;
	if (fileMD5.length() != 0)
	{
		tmp_stringlist.push_back("Content-MD5-Type: " + fileMD5);
	}




	int hostend = storageURL.find("/", 8);
	if (hostend <= 0)
	{
		return FarmbeatsError_UrlError;
	}

	std::string storageHost = storageURL.substr(8, hostend - 8);

	std::string storageTargetResource = storageURL.substr(hostend, storageURL.length() - hostend);


	int ret = httpRequest_Uploadfile(
		storageHost,
		std::string("Blob API UploadFile"),
		std::string("PUT"),
		storageTargetResource,
		tmp_stringlist,
		filepath,
		JsonResult
	);



	if (ret != FarmbeatsError_Success)
	{
		return FarmbeatsError_UploadfileFailed;
	}
	return FarmbeatsError_Success;
}

int FarmbeatsapiImpl::ParseResult_Login(std::string JsonResult, std::string& Token)
{

	rapidjson::Document doc4Result;
	if (doc4Result.Parse(JsonResult.c_str()).HasParseError())
	{
		return FarmbeatsError_AuthenticationError;
	}

	if (doc4Result.HasMember("token_type") && doc4Result.HasMember("access_token"))
	{
		Token.clear();
		rapidjson::Value& token_type = doc4Result["token_type"];
		Token += token_type.GetString() + std::string(" ");
		rapidjson::Value& access_token = doc4Result["access_token"];
		Token += access_token.GetString();
		return FarmbeatsError_Success;
	}
	else
	{
		return FarmbeatsError_AuthenticationError;
	}

}
int FarmbeatsapiImpl::ParseResult_GetFarmList(std::string JsonResult, std::string& farmlistJson)
{
	rapidjson::Document doc4Result;
	//(doc4Result.Parse<rapidjson::kParseNumbersAsStringsFlag>(JsonResult.c_str()).HasParseError()
	if (doc4Result.Parse(JsonResult.c_str()).HasParseError())
	{
		return FarmbeatsError_AuthenticationError;
	}

	rapidjson::StringBuffer buffer_NewJson;

	rapidjson::Writer<rapidjson::StringBuffer> writer_NewJson(buffer_NewJson);
	writer_NewJson.StartObject();
	writer_NewJson.Key("Farms");
	writer_NewJson.StartArray();
	if (doc4Result.HasMember("items"))
	{
		rapidjson::Value& items = doc4Result["items"];
		if (items.Size() == 0)
		{
			farmlistJson = "{\"Farm\":[]}";
			return FarmbeatsError_Success;
		}
		for (auto& item : items.GetArray())
		{
			writer_NewJson.StartObject();

			//name
			if (item.HasMember("name"))
			{
				writer_NewJson.Key("name");
				writer_NewJson.String(item["name"].GetString());
			}


			//id
			if (item.HasMember("id"))
			{
				writer_NewJson.Key("id");
				writer_NewJson.String(item["id"].GetString());
			}
			else
			{

				//to do : log the error 

			}
			//createdAt
			if (item.HasMember("createdAt"))
			{
				writer_NewJson.Key("createdAt");
				writer_NewJson.String(item["createdAt"].GetString());
			}
			else
			{

				//to do : log the error 

			}

			//geometry
			if (item.HasMember("geometry"))
			{
				rapidjson::Value& geometry = item["geometry"];
				writer_NewJson.Key("geometry");
				writer_NewJson.StartObject();
				//type
				if (geometry.HasMember("type"))
				{
					writer_NewJson.Key("type");
					writer_NewJson.String(geometry["type"].GetString());
				}
				//coordinates
				if (geometry.HasMember("coordinates"))
				{
					writer_NewJson.Key("coordinates");
					writer_NewJson.StartArray();
					auto& locations = (geometry["coordinates"].GetArray())[0];

					for (int i = 0; i < (locations.Size() - 1); i++) //减一，去除最后一个重复的经纬度。
					{//auto& location : locations.GetArray()
						auto& location = locations[i];
						if (location.Size() > 1)
						{
							writer_NewJson.StartObject();
							//注意这里有没有写反数据
							writer_NewJson.Key("lng"); writer_NewJson.Double(location[0].GetDouble());
							writer_NewJson.Key("lat"); writer_NewJson.Double(location[1].GetDouble());
							writer_NewJson.EndObject();
						}
					}



					writer_NewJson.EndArray();
				}

				writer_NewJson.EndObject();
			}



			writer_NewJson.EndObject();
		}


	}
	else
	{
		return FarmbeatsError_JsonParseError;
	}
	writer_NewJson.EndArray();
	writer_NewJson.EndObject();

	farmlistJson = buffer_NewJson.GetString();
	return FarmbeatsError_Success;
}
int FarmbeatsapiImpl::ParseReslt_CheckExtendedTypeExists(std::string JsonResult, std::string extendedTypeKey, std::string extendedTypeValue, bool& extendedTypeExists, std::string& extendedTypeID, std::string& extendTypeBody)
{
	rapidjson::Document doc4Result;
	if (doc4Result.Parse<rapidjson::kParseNumbersAsStringsFlag>(JsonResult.c_str()).HasParseError())
	{
		return FarmbeatsError_AuthenticationError;
	}

	rapidjson::StringBuffer buffer_NewJson;

	rapidjson::Writer<rapidjson::StringBuffer> writer_NewJson(buffer_NewJson);
	writer_NewJson.StartObject();


	if (doc4Result.HasMember("items"))
	{
		rapidjson::Value& items = doc4Result["items"];
		if (items.Size() == 0)
		{
			//std::cout << "items is none";
			//新的farmbeats会不会没有这个list，如果这样会有问题，所以单独列出一个错误，以方便后面调试
			return FarmbeatsError_ExtendedTypeListIsNULL;
		}
		auto itemsArray = items.GetArray();
		for (auto item = itemsArray.Begin(); item != itemsArray.End(); ++item)
		{


			//key
			if (item->HasMember("key"))
			{
				writer_NewJson.Key("key");
				std::string tmp_key = (*item)["key"].GetString();
				if (0 != tmp_key.compare(extendedTypeKey))
				{
					return FarmbeatsError_ExtendedTypeMismatch;
				}
				writer_NewJson.String((*item)["key"].GetString());

			}


			//id
			if (item->HasMember("id"))
			{
				//std::cout << "id=" << (*item)["id"].GetString() << std::endl;
				extendedTypeID = (*item)["id"].GetString();
			}

			//description
			if (item->HasMember("description"))
			{
				writer_NewJson.Key("description");
				writer_NewJson.String((*item)["description"].GetString());
			}
			//value
			if (item->HasMember("value"))
			{
				auto typeValue = (*item)["value"].GetArray();
				writer_NewJson.Key("value");
				writer_NewJson.StartArray();

				for (auto valueItem = typeValue.Begin(); valueItem != typeValue.End(); ++valueItem)
				{
					if (0 == extendedTypeValue.compare(valueItem->GetString()))
					{

						extendedTypeExists = true;
						return FarmbeatsError_Success;
					}
					else
					{
						writer_NewJson.String(valueItem->GetString());
					}
				}
				writer_NewJson.String(extendedTypeValue.c_str());
				writer_NewJson.EndArray();
			}

		}


	}
	else
	{
		return FarmbeatsError_ParamsParseError;
	}
	extendedTypeExists = false;
	writer_NewJson.EndObject();
	extendTypeBody = buffer_NewJson.GetString();
	return FarmbeatsError_Success;
}
int FarmbeatsapiImpl::ParseResult_CreatMission(std::string JsonResult, std::string& MissionID)
{

	rapidjson::Document doc4Result;
	if (doc4Result.Parse(JsonResult.c_str()).HasParseError())
	{
		return FarmbeatsError_AuthenticationError;
	}

	if (doc4Result.HasMember("id"))
	{
		MissionID = doc4Result["id"].GetString();

		return FarmbeatsError_Success;
	}
	else
	{
		return FarmbeatsError_AuthenticationError;
	}

}
int FarmbeatsapiImpl::ParseResult_GetSceneFileURL(std::string JsonResult, std::string& storageURL, std::string& fileID)
{

	rapidjson::Document doc4Result;
	if (doc4Result.Parse(JsonResult.c_str()).HasParseError())
	{
		return FarmbeatsError_AuthenticationError;
	}

	if (doc4Result.HasMember("uploadSASUrl") && doc4Result.HasMember("id"))
	{
		storageURL = doc4Result["uploadSASUrl"].GetString();
		fileID = doc4Result["id"].GetString();
		return FarmbeatsError_Success;
	}
	else
	{
		return FarmbeatsError_AuthenticationError;
	}


}

int FarmbeatsapiImpl::httpRequest(std::string requestUrl, std::string requstName, std::string httpVerb, std::string  targetResource, std::string requestHeader, std::string requestBody, std::string& responseContent)
{
	std::list<std::string> tmp_stringlist;
	tmp_stringlist.push_front(requestHeader);
	return httpRequest(requestUrl, requstName, httpVerb, targetResource, tmp_stringlist, requestBody, responseContent);

}
int FarmbeatsapiImpl::httpRequest(std::string requestUrl, std::string requstName, std::string httpVerb, std::string  targetResource, std::list<std::string> requestHeaders, std::string requestBody, std::string& responseContent)
{
	BOOL  bResults = FALSE;

	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

	LPSTR pszOutBuffer = NULL;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;


	std::wstring hostwstr = string2wstring(requestUrl); // input your host here ,for example:  "southeastasia.api.cognitive.microsoft.com"
	std::wstring requstNamewstr = string2wstring(requstName);
	hSession = WinHttpOpen(requstNamewstr.c_str(),
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);
	

	
	// Connect an HTTPS server.  INTERNET_DEFAULT_HTTPS_PORT  means using 443 port. You can change to  INTERNET_DEFAULT_HTTP_PORT ,when you connect to an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, hostwstr.c_str(),
			INTERNET_DEFAULT_HTTPS_PORT, 0);
	else
	{
		//int err = GetLastError();
		// Close handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		int err = GetLastError();
		if (err == ERROR_WINHTTP_TIMEOUT)
		{
			return FarmbeatsError_RequestTimeOut;
		}
		return FarmbeatsError_CommunicationError;
	}

	std::wstring httpVerbwstr = string2wstring(httpVerb);
	std::wstring targetResourcewstr = string2wstring(targetResource);
	// Create an HTTPS Request.    WINHTTP_FLAG_SECURE means using SSL.  You can change to  0x0 ,when you connect to an HTTP server.
	if (hConnect)
		hRequest = WinHttpOpenRequest(
			hConnect,
			httpVerbwstr.c_str(),
			targetResourcewstr.c_str(), //  In REST API , what content you write means what feature you choose , such as "/customvision/v3.0/Prediction/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/classify/iterations/Iteration1/image"
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);

	if (!hConnect||!hRequest)
	{
		// Close handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		return FarmbeatsError_CommunicationError;
	}


	BOOL bRet = WinHttpSetTimeouts(hRequest, RESOLVE_TIMEOUT, CONNECT_TIMEOUT, SEND_TIMEOUT, RECEIVE_TIMEOUT);
	
	// Set Options , client without certificates. You can set other Options ,such as proxy, timeout and so on.
	bRet = WinHttpSetOption(hRequest, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, WINHTTP_NO_CLIENT_CERT_CONTEXT, 0);

	for (auto requestHeaderitem = requestHeaders.begin(); requestHeaderitem != requestHeaders.end(); ++requestHeaderitem)
	{
		std::wstring requestHeaderitemwstr = string2wstring(*requestHeaderitem);
		if (hRequest)
			bResults = WinHttpAddRequestHeaders(hRequest,
				requestHeaderitemwstr.c_str(),   //other informations in the form when using http "post".  "Prediction - Key" is a field for MS Cognitive service REST API ,if you use other API,you should input their specific fields. 
				(ULONG)-1L,
				WINHTTP_ADDREQ_FLAG_ADD);
		
	}


	//if (hRequest)
	//	bResults = WinHttpAddRequestHeaders(hRequest,
	//		L"Content-Type: application/octet-stream",  //other informations in the form when using http "post",same explanation as the last step.For example: "Content-Type: application/octet-stream".
	//		(ULONG)-1L,
	//		WINHTTP_ADDREQ_FLAG_ADD);


	//ReadFile();

	//Send the Request
	if (0 != requestBody.length())
	{
		const void* data_c_str = (const char*)requestBody.c_str();
		//Send the Request
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			const_cast<void*>(data_c_str), requestBody.length(), requestBody.length(), 0);
	}
	else
	{
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0, 0,
			0, 0, 0);
	}

	

	// Receive the Response
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	else
	{
		int err = GetLastError();
		// Close handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		if (err == ERROR_WINHTTP_TIMEOUT)
		{
			return FarmbeatsError_RequestTimeOut;
		}
		return FarmbeatsError_CommunicationError;
	}

	std::string JsonResult;
	

	do
	{
		// Verify.
		dwSize = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize));
		//std::cout << "Error %u in WinHttpQueryDataAvailable.\n" << GetLastError();

	// new a space for the buffer.
		pszOutBuffer = new char[dwSize + 1];
		if (!pszOutBuffer)
		{
			//std::cout << pszOutBuffer; "Out of memory\n";
			dwSize = 0;
			return FarmbeatsError_OutOfMemory;
		}
		else
		{
			// Read the Data.
			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded));

			else
			{
				if (dwSize != 0)
				{
					JsonResult += pszOutBuffer;
				}

			}

			// delete the memory .
			if (pszOutBuffer)
			{
				delete[] pszOutBuffer;
			}
		}

	} while (dwSize > 0);





	// Close handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);


	responseContent = JsonResult;
	return FarmbeatsError_Success;
}
int FarmbeatsapiImpl::httpRequest_Uploadfile(std::string requestUrl, std::string requstName, std::string httpVerb, std::string  targetResource, std::list<std::string> requestHeaders, std::string requestBody, std::string& responseContent)
{
	BOOL  bResults = FALSE;

	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

	LPSTR pszOutBuffer = NULL;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;



	std::wstring hostwstr = string2wstring(requestUrl); // input your host here ,for example:  "southeastasia.api.cognitive.microsoft.com"
	std::wstring requstNamewstr = string2wstring(requstName);
	hSession = WinHttpOpen(requstNamewstr.c_str(),
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Set Options for time-out
	/*
	#define WINHTTP_OPTION_CONNECT_TIMEOUT                  3
	#define WINHTTP_OPTION_SEND_TIMEOUT                     5
	#define WINHTTP_OPTION_RECEIVE_TIMEOUT                  6
	#define WINHTTP_OPTION_RECEIVE_RESPONSE_TIMEOUT         7
	*/





	// Connect an HTTPS server.  INTERNET_DEFAULT_HTTPS_PORT  means using 443 port. You can change to  INTERNET_DEFAULT_HTTP_PORT ,when you connect to an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, hostwstr.c_str(),
			INTERNET_DEFAULT_HTTPS_PORT, 0);
	else
	{
		// Close handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		
		return FarmbeatsError_CommunicationError;
	}

	std::wstring httpVerbwstr = string2wstring(httpVerb);
	std::wstring targetResourcewstr = string2wstring(targetResource);
	// Create an HTTPS Request.    WINHTTP_FLAG_SECURE means using SSL.  You can change to  0x0 ,when you connect to an HTTP server.
	if (hConnect)
		hRequest = WinHttpOpenRequest(
			hConnect,
			httpVerbwstr.c_str(),
			targetResourcewstr.c_str(), //  In REST API , what content you write means what feature you choose , such as "/customvision/v3.0/Prediction/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx/classify/iterations/Iteration1/image"
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);

	if (!hConnect || !hRequest)
	{
		// Close handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		
		return FarmbeatsError_CommunicationError;
	}

	BOOL bRet = WinHttpSetTimeouts(hRequest, UPLOADFILE_RESOLVE_TIMEOUT, UPLOADFILE_CONNECT_TIMEOUT, UPLOADFILE_SEND_TIMEOUT, UPLOADFILE_RECEIVE_TIMEOUT);

	// Set Options , client without certificates. You can set other Options ,such as proxy, timeout and so on.
	bRet = WinHttpSetOption(hRequest, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, WINHTTP_NO_CLIENT_CERT_CONTEXT, 0);
	for (auto requestHeaderitem = requestHeaders.begin(); requestHeaderitem != requestHeaders.end(); ++requestHeaderitem)
	{
		std::wstring requestHeaderitemwstr = string2wstring(*requestHeaderitem);
		if (hRequest)
			bResults = WinHttpAddRequestHeaders(hRequest,
				requestHeaderitemwstr.c_str(),   //other informations in the form when using http "post".  "Prediction - Key" is a field for MS Cognitive service REST API ,if you use other API,you should input their specific fields. 
				(ULONG)-1L,
				WINHTTP_ADDREQ_FLAG_ADD);
	}


	//if (hRequest)
	//	bResults = WinHttpAddRequestHeaders(hRequest,
	//		L"Content-Type: application/octet-stream",  //other informations in the form when using http "post",same explanation as the last step.For example: "Content-Type: application/octet-stream".
	//		(ULONG)-1L,
	//		WINHTTP_ADDREQ_FLAG_ADD);


	//ReadFile();


	char* buffer = NULL;
	long  lSize = 0;
	bool bReadfileReady = false;
	try
	{
		//Read Picture
		FILE* pFile = NULL;
		size_t result = 0;;
		fopen_s(&pFile, requestBody.c_str(), "rb");
		if (pFile != NULL)
		{
			fseek(pFile, 0, SEEK_END);
			lSize = ftell(pFile);
			rewind(pFile);
			buffer = (char*)malloc(sizeof(char) * lSize);
			if (buffer != NULL)
			{
				result = fread(buffer, 1, lSize, pFile);
			
				bReadfileReady = true;
			}	
		}

		fclose(pFile);


	}
	catch (...)
	{
		bReadfileReady = false;
	}

	if (bReadfileReady == false)
	{
		if (buffer != NULL)
		{
			free(buffer);
			buffer = NULL;
		}

		return FarmbeatsError_ReadfileFailed;
	}


	//Send the Request
	if (0 != requestBody.length())
	{
		const void* data_c_str = (const char*)buffer;
		//Send the Request
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			const_cast<void*>(data_c_str), lSize, lSize, 0);
	}
	else
	{
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0, 0,
			0, 0, 0);
	}

	// Receive the Response
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);


	if (!bResults)
	{
		if (buffer != NULL)
		{
			free(buffer);
			buffer = NULL;
		}
		// Close handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);


		int err = GetLastError();
		if (err == ERROR_WINHTTP_TIMEOUT)
		{
			return FarmbeatsError_RequestTimeOut;
		}
		return FarmbeatsError_CommunicationError;
	}

	std::string JsonResult;
	

	do
	{
		// Verify.
		dwSize = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize));
		//std::cout << "Error %u in WinHttpQueryDataAvailable.\n" << GetLastError();

	// new a space for the buffer.
		pszOutBuffer = new char[dwSize + 1];
		if (!pszOutBuffer)
		{
			//std::cout << pszOutBuffer; "Out of memory\n";
			dwSize = 0;
			return FarmbeatsError_OutOfMemory;
		}
		else
		{
			// Read the Data.
			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded));

			else
			{
				if (dwSize != 0)
				{
					JsonResult += pszOutBuffer;
				}

			}

			// delete the memory .
			if (pszOutBuffer)
			{
				delete[] pszOutBuffer;
			}
		}

	} while (dwSize > 0);

	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}



	// Close handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);


	responseContent = JsonResult;
	return FarmbeatsError_Success;
}

int FarmbeatsapiImpl::ImageConvert2PNG(std::string sourceFile, std::string  destFile)
{
	try 
	{
		cv::Mat img = cv::imread(sourceFile, -1);
		std::vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);
		bool ret = imwrite(destFile, img,compression_params);
		return ret ? FarmbeatsError_Success : FarmbeatsError_ImageConvertFailed;
	}
	catch(...)
	{
		return FarmbeatsError_ImageConvertFailed;
	}
	
}

int FarmbeatsapiImpl::ImageCompression(std::string sourceFile, std::string& destFile)
{
	long  lSize = fileSize(sourceFile);
	if (lSize == -1)
	{
		return FarmbeatsError_ReadfileFailed;
	}
	if (lSize < ImageSizeMaxlimit)
	{
		destFile = sourceFile;
		return  FarmbeatsError_Success;
	}
	destFile = sourceFile.substr(0, sourceFile.length() - 4) + "Compressed.png";
	cv::Mat img = cv::imread(sourceFile, -1);
	cv::Size imgsize = img.size();
	int width = imgsize.width;
	int height = imgsize.height;

	while (1)
	{

		float factor = 0.97 * cv::sqrt(float(ImageSizeMaxlimit) / float(lSize));

		if (factor > 0.97)
		{
			factor = 0.95;
		}
		int destwidth = int(float(width) * factor);
		int destheight = int(float(height) * factor);

		try
		{
			cv::Size destsize(destwidth, destheight);
			cv::Mat destimg;
			cv::resize(img, destimg, destsize);
			cv::imwrite(destFile, destimg);
			imgsize = destimg.size();
		}
		catch (...)
		{

		}

		lSize = fileSize(destFile);

		if (lSize == -1)
		{
			return FarmbeatsError_ImageConvertFailed;
		}

		if (lSize < ImageSizeMaxlimit)
		{

			break;
		}

		img = cv::imread(destFile, -1);
		width = imgsize.width;
		height = imgsize.height;
	}

	return FarmbeatsError_Success;
}
long FarmbeatsapiImpl::fileSize(std::string sourceFile)
{
	//Read Picture
	long  lSize = 0;
	try
	{
		FILE* pFile = NULL;
		size_t result = 0;;
		fopen_s(&pFile, sourceFile.c_str(), "rb");
		if (pFile != NULL)
		{
			fseek(pFile, 0, SEEK_END);
			lSize = ftell(pFile);
		}
		fclose(pFile);
	}
	catch (...)
	{
		lSize = -1;
	}



	return lSize;
}
std::string FarmbeatsapiImpl::wstring2string(std::wstring wstr)
{
	std::string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;

}
std::wstring FarmbeatsapiImpl::string2wstring(std::string str)
{
	std::wstring result;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;

}
std::string FarmbeatsapiImpl::GetTimeString()
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	char timestr[28] = { 0 };
	sprintf_s(timestr,
		"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	return std::string(timestr);
}

