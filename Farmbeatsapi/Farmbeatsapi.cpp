#pragma once
#include "pch.h"
#include "Farmbeatsapi.h"
#include "FarmbeatsapiImpl.h"
extern "C" FARMBEATSAPI IFarmbeatsapi * WINAPI GetFarmbeatsapi();
extern "C" FARMBEATSAPI void WINAPI DestoryFarmbeatsapi(IFarmbeatsapi * hfarmbeatsapi);
class  Farmbeatsapi :public IFarmbeatsapi
{
public:
	Farmbeatsapi();
	~Farmbeatsapi();
	virtual int  Login(std::string fbhub_url, std::string clientID, std::string clientSecret, std::string tenantID);
	virtual int  GetFarmList(std::string& farmlistJson);
	virtual int  CreatMission(std::string farmID, std::string& MissionID);
	virtual int  UploadFile(std::string missionID, std::string filename, std::string filepath, Farmbeats_ImageType imageType, std::string fileMD5 = "");
	virtual std::string  FBGetAPIVersion();

private:
	FarmbeatsapiImpl* m_farmbeatsapiimpl = NULL;
	std::string m_Token;
	std::string m_fbhub_url;
	std::string m_fbhub_url_host;
	std::string m_clientID;
	std::string m_clientSecret;
	std::string m_tenantID;
	bool m_bCheckExtendedTypeExists = false;
};

Farmbeatsapi::Farmbeatsapi()
{
	m_farmbeatsapiimpl = new FarmbeatsapiImpl();
}
Farmbeatsapi::~Farmbeatsapi()
{
	if (m_farmbeatsapiimpl)
	{
		delete m_farmbeatsapiimpl;
		m_farmbeatsapiimpl = NULL;
	}
}

std::string Farmbeatsapi::FBGetAPIVersion()
{
	return std::string("0.0.3");
}
int Farmbeatsapi::Login(std::string fbhub_url, std::string clientID, std::string clientSecret, std::string tenantID)
{
	if (!m_farmbeatsapiimpl)
		return FarmbeatsError_UnknowError;

	if (fbhub_url.length() == 0 && clientID.length() == 0 && clientSecret.length() == 0 && tenantID.length() == 0)
		return FarmbeatsError_ParamsError;

	m_fbhub_url = fbhub_url;
	m_clientID = clientID;
	m_clientSecret = clientSecret;
	m_tenantID = tenantID;
	int ret = FarmbeatsError_UnknowError;
	try
	{
		ret = m_farmbeatsapiimpl->Login(fbhub_url, clientID, clientSecret, tenantID, m_Token);
	}
	catch (...)
	{
		return FarmbeatsError_UnknowError;
	}
	if (ret == FarmbeatsError_Success)
	{
		m_fbhub_url_host = m_fbhub_url.substr(8, m_fbhub_url.length() - 8);
	}
	return ret;
}
int Farmbeatsapi::GetFarmList(std::string& farmlistJson)
{
	if (!m_farmbeatsapiimpl)
		return FarmbeatsError_UnknowError;

	int ret = m_farmbeatsapiimpl->GetFarmList(m_fbhub_url_host, m_Token, farmlistJson);
	if (ret == FarmbeatsError_AuthenticationError)
	{
		if (m_farmbeatsapiimpl->Login(m_fbhub_url, m_clientID, m_clientSecret, m_tenantID, m_Token) != FarmbeatsError_Success)
		{
			return FarmbeatsError_AuthenticationError;
		}
		else
		{
			ret = m_farmbeatsapiimpl->GetFarmList(m_fbhub_url_host, m_Token, farmlistJson);
		}

	}

	return ret;
}
int Farmbeatsapi::CreatMission(std::string farmID, std::string& MissionID)
{

	int ret = FarmbeatsError_UnknowError;

	if (!m_farmbeatsapiimpl)
		return ret;

	if (farmID.length() == 0)
	{
		return FarmbeatsError_ParamsError;
	}

	if (m_bCheckExtendedTypeExists == false)
	{
		bool bcheck = false;
		std::string extendtypebody;
		std::string extendtypeID;
		/*
		› SceneType: drone
		› SceneFileType: orthomosaic
		› SceneFileContentType: image/tiff
		› SceneSource: DJI
		*/
		std::map<std::string, std::string> tmp_map;
		tmp_map["SceneType"] = "drone";
		tmp_map["SceneFileType"] = "orthomosaic";
		tmp_map["SceneFileContentType"] = "image/tiff";
		tmp_map["SceneFileContentType"] = "image/png";
		tmp_map["SceneSource"] = "DJI";
		for (auto mapitem = tmp_map.begin(); mapitem != tmp_map.end(); ++mapitem)
		{
			ret = FarmbeatsError_UnknowError;
			ret = m_farmbeatsapiimpl->CheckExtendedTypeExists(m_fbhub_url_host, m_Token, mapitem->first, mapitem->second, bcheck, extendtypeID, extendtypebody);
			if (ret == FarmbeatsError_AuthenticationError)
			{
				if (m_farmbeatsapiimpl->Login(m_fbhub_url, m_clientID, m_clientSecret, m_tenantID, m_Token) != FarmbeatsError_Success)
				{
					return FarmbeatsError_AuthenticationError;
				}
				else
				{
					ret = m_farmbeatsapiimpl->CheckExtendedTypeExists(m_fbhub_url_host, m_Token, mapitem->first, mapitem->second, bcheck, extendtypeID, extendtypebody);
				}
			}
			if (ret != FarmbeatsError_Success)
			{
				return ret;
			}

			if (bcheck == false)
			{

				ret = m_farmbeatsapiimpl->AddExtenedType(m_fbhub_url_host, m_Token, extendtypeID, extendtypebody);
				if (ret == FarmbeatsError_AuthenticationError)
				{
					if (m_farmbeatsapiimpl->Login(m_fbhub_url, m_clientID, m_clientSecret, m_tenantID, m_Token) != FarmbeatsError_Success)
					{
						return FarmbeatsError_AuthenticationError;
					}
					else
					{
						ret = m_farmbeatsapiimpl->AddExtenedType(m_fbhub_url_host, m_Token, extendtypeID, extendtypebody);
					}
				}
			}

			if (ret != FarmbeatsError_Success)
			{
				return ret;
			}


		}

		m_bCheckExtendedTypeExists = true;


	}


	ret = m_farmbeatsapiimpl->CreatMission(m_fbhub_url_host, m_Token, farmID, MissionID);
	if (ret == FarmbeatsError_AuthenticationError)
	{
		if (m_farmbeatsapiimpl->Login(m_fbhub_url, m_clientID, m_clientSecret, m_tenantID, m_Token) != FarmbeatsError_Success)
		{
			return FarmbeatsError_AuthenticationError;
		}
		else
		{
			ret = m_farmbeatsapiimpl->CreatMission(m_fbhub_url_host, m_Token, farmID, MissionID);
		}
	}

	return ret;
}
int Farmbeatsapi::UploadFile(std::string missionID, std::string filename, std::string filepath, Farmbeats_ImageType imageType, std::string fileMD5)
{

#ifdef EXECTIME

#endif // EXECTIME

#ifdef EXECTIME
	DWORD start_time;
	DWORD end_time;
#endif // EXECTIME


	int ret = FarmbeatsError_UnknowError;

	if (!m_farmbeatsapiimpl)
		return ret;

	if (missionID.length() == 0 || filename.length() <= 4 || filepath.length() <= 4)
	{
		return FarmbeatsError_ParamsError;
	}

#ifdef EXECTIME
	start_time = GetTickCount();
#endif // EXECTIME
	if (imageType != Image_TIFF)
	{
		if (imageType != Image_PNG)
		{

			std::string destfile = filepath.substr(0, filepath.length() - 4) + ".png";

			ret = m_farmbeatsapiimpl->ImageConvert2PNG(filepath, destfile);
			if (ret != FarmbeatsError_Success)
				return ret;

			filepath = destfile;
			filename = filename.substr(0, filename.length() - 4) + ".png";

		}
#ifdef EXECTIME
		end_time = GetTickCount();

		std::cout << "ImageConvert2PNG time is:" << (end_time - start_time) << "ms!" <<std::endl;
		std::cout << std::endl;
#endif // EXECTIME

#ifdef EXECTIME
		start_time = GetTickCount();
#endif // EXECTIME
		std::string destfile;
		ret = m_farmbeatsapiimpl->ImageCompression(filepath, destfile);
		if (ret != FarmbeatsError_Success)
			return ret;
		if (filepath.compare(destfile) != 0)
		{
			//compression  , md5 will change.
			fileMD5.clear();
		}
		filepath = destfile;
	}
#ifdef EXECTIME
	end_time = GetTickCount();

	std::cout << "ImageCompression time is:" << (end_time - start_time) << "ms!" << std::endl;
	std::cout << std::endl;

#endif // EXECTIME

#ifdef EXECTIME
	start_time = GetTickCount();
#endif // EXECTIME

	std::string storageURL;
	std::string fileID;
	ret = m_farmbeatsapiimpl->GetSceneFileURL(m_fbhub_url_host, m_Token, missionID, filename, filepath, storageURL, fileID, imageType);
	if (ret == FarmbeatsError_AuthenticationError)
	{
		if (m_farmbeatsapiimpl->Login(m_fbhub_url, m_clientID, m_clientSecret, m_tenantID, m_Token) != FarmbeatsError_Success)
		{
			return FarmbeatsError_AuthenticationError;
		}
		else
		{
			ret = m_farmbeatsapiimpl->GetSceneFileURL(m_fbhub_url_host, m_Token, missionID, filename, filepath, storageURL, fileID, imageType);
		}
	}
	if (ret != FarmbeatsError_Success)
	{
		return ret;
	}

#ifdef EXECTIME
	end_time = GetTickCount();

	std::cout << "GetSceneFileURL time is:" << (end_time - start_time) << "ms!" << std::endl;
	std::cout << std::endl;

#endif // EXECTIME



#ifdef EXECTIME
	start_time = GetTickCount();
#endif // EXECTIME

	ret = m_farmbeatsapiimpl->UploadFile(storageURL, m_Token, filename, filepath, fileMD5, imageType);

#ifdef EXECTIME
	end_time = GetTickCount();

	std::cout << "UploadFile time is:" << (end_time - start_time) << "ms!" << std::endl;
	std::cout << std::endl;

#endif // EXECTIME

	if (ret != FarmbeatsError_Success)
	{
		m_farmbeatsapiimpl->DeleteSceneFile(m_fbhub_url_host, m_Token, fileID);
	}

	return ret;

}

FARMBEATSAPI void WINAPI DestoryFarmbeatsapi(IFarmbeatsapi* hfarmbeatsapi)
{
	if (hfarmbeatsapi != NULL)
	{
		delete((Farmbeatsapi*)hfarmbeatsapi);
		hfarmbeatsapi = NULL;
	}
}

FARMBEATSAPI IFarmbeatsapi* WINAPI GetFarmbeatsapi()
{
	return new Farmbeatsapi();
}