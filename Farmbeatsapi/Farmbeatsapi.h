#pragma once
#include <iostream>
#include <Windows.h>
#ifdef FARMBEATSAPI_EXPORTS

#define FARMBEATSAPI _declspec(dllexport)

#else

#define FARMBEATSAPI _declspec(dllimport)

#endif


//#define EXECTIME

#define RESOLVE_TIMEOUT 30000
#define CONNECT_TIMEOUT 30000
#define SEND_TIMEOUT	30000
#define RECEIVE_TIMEOUT 30000

#define UPLOADFILE_RESOLVE_TIMEOUT 190000
#define UPLOADFILE_CONNECT_TIMEOUT 190000
#define UPLOADFILE_SEND_TIMEOUT	190000
#define UPLOADFILE_RECEIVE_TIMEOUT 190000


#define ImageSizeMaxlimit 100 *1000 * 1000 //byte


#define     FarmbeatsError_Success          0   //�ɹ�
#define     FarmbeatsError_UnknowError      -1  //δ֪����
#define     FarmbeatsError_AuthenticationError -2 //��֤ʧ��
#define     FarmbeatsError_CommunicationError -3      //ͨѶʧ��
#define     FarmbeatsError_UrlError           -4    //URL����
#define     FarmbeatsError_ParamsParseError   -5    //������������
#define     FarmbeatsError_ParamsError        -6    //��������
#define     FarmbeatsError_JsonParseError   -7    //Json��������
#define     FarmbeatsError_OutOfMemory   -8    //�ڴ治��
#define     FarmbeatsError_ExtendedTypeListIsNULL   -9    //��ѯ��ExtendedType�б�Ϊ��
#define     FarmbeatsError_ExtendedTypeMismatch   -10    //��ѯ��ExtendedType������ƥ��
#define		FarmbeatsError_ReadfileFailed	-11			//��ȡ�ļ�ʧ��
#define		FarmbeatsError_UploadfileFailed	-12			//�ϴ��ļ�ʧ��
#define		FarmbeatsError_RequestTimeOut	-13		//time out
#define		FarmbeatsError_ImageConvertFailed	-14
#define		FarmbeatsError_ImageCompressionFailed	-15

//#define   //GetLastError =  12019; ERROR_WINHTTP_INCORRECT_HANDLE_STATE 


typedef enum
{
	Image_PNG = 0,
	Image_JPG ,
	Image_TIFF

}Farmbeats_ImageType;


class IFarmbeatsapi
{
public:
	
	virtual int  Login(std::string fbhub_url, std::string clientID, std::string clientSecret, std::string tenantID) = 0;
	virtual int  GetFarmList(std::string& farmlistJson) = 0;
	virtual int  CreatMission(std::string farmID, std::string& MissionID) = 0;
	virtual int  UploadFile(std::string missionID, std::string filename, std::string filepath, Farmbeats_ImageType imageType,std::string fileMD5= "") = 0;
	virtual std::string  FBGetAPIVersion() = 0;

};





