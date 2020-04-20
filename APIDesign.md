# Farmbeats API
|Version|Data|
|:---|:---|
|0.0.3|2020/2/20|


- 为了快速实现对接Farmbeats的功能，这个API把Farmbeats相关内容封装成动态库。
- API包含了登录Farmbeats，获取Farm列表，对某个Farm创建飞行任务，以及上传某次飞行的图片等功能。
## 内容
- DLL
    - 导出类的创建函数。
```C++
// 创建Farmbeatsapi对象实例的函数
extern "C" FARMBEATSAPI IFarmbeatsapi*  WINAPI GetFarmbeatsapi();


// 释放Farmbeatsapi对象实例的函数

extern "C" FARMBEATSAPI void WINAPI DestoryFarmbeatsapi(IFarmbeatsapi * hfarmbeatsapi);


```
- 头文件
    - 抽象类定义 IFarmbeatsapi
    - 入参出参结构体定义
    - 错误码定义
## 接口定义
###  登录Farmbeats

```C++
int Farmbeatsapi::Login(std::string fbhub_url,std::string clientID,std::string clientSecret,std::string tenantID)
{
//fbhub_url = "https://xxxxx.azurewebsites.net"
//clientID = "38a28cec-XXXX-XXXX-XXXX-0b128ef14c71"
//clientSecret = "QXXXXo*[]XXXXXXXX++wJvB4Qf5zDeP]"
//tenantID = "72f988bf-XXXX-XXXX-XXXX-2d7cd011db47"
}

```
   - 输入 
        - fbhub_url,clientID,clientSecret,tenantID;
   - 返回
        - 0为成功
        - 非零详见错误码定义
   - 说明
        - 同步，且当前版本不支持取消。
        - 每次调用login都需要传入相应参数来获取。
        - 可以重复调用。
        - 支持自动时效刷新。
        - 暂无相对应的Logout().

### 获取Farmbeats中的Farm列表
```C++
int Farmbeatsapi::GetFarmList(std::string & farmlistJson)
{

}

/*Farm List Json
{
   "Farms":[
      {
         "name":"FuGui",
         "id":"234564",
         "createdAt": "2020-02-19T14:50:56.4139884Z",
         "geometry":{
            "type":"Polygon",
            "coordinates":[
               {
                  "lng":113.93725817546249,
                  "lat":22.526812288640446
               },
               {
                  "lng":113.94039848044514,
                  "lat":22.52675762351154
               }
            ]
         }
      },
      {
         "name":"DuoFu",
         "id":"54654",
         "createdAt": "2020-02-19T14:50:56.4139884Z",
         "geometry":{
            "type":"Polygon",
            "coordinates":[
               {
                  "lng":113.93725817546249,
                  "lat":22.526812288640446
               },
               {
                  "lng":113.94039848044514,
                  "lat":22.52675762351154
               }
            ]
         }
      }
   ]
}
*/

```
   - 输入 
      - std::string farmlistJson, 用于存储返回的农场列表。
   - 返回
      - 0为成功
      - 非零详见错误码定义
   - 说明
      - 同步，且当前版本不支持取消。
      - 每个Farm都有三个信息，名字，ID，geometry。其中geometry中的格式是与原来一致的。

### 对某个农场创建飞行任务
```C++

int Farmbeatsapi::CreatMission(std::string farmID , std::string & MissionID)
{

}
```
   - 输入 
      - std::string & MissionID, 用于存储返回的飞行任务ID。
   - 返回
        - 0为成功
        - 非零详见错误码定义
   - 说明
        - 同步，且当前版本不支持取消。
        - 这个ID会在上传文件时用到。
        
### 上传飞行任务的结果文件
```C++

typedef enum
{
	Image_PNG = 0, 
	Image_JPG ,
	Image_TIFF

}Farmbeats_ImageType;

int Farmbeatsapi::UploadFile(std::string missionID,std::string filename,std::string filepath, Farmbeats_ImageType imageType,std::string fileMD5 = "");
{

}

```
   - 输入 
        - std::string missionID -- 飞行任务ID
        - std::string filename -- 文件名
        - std::string filepath -- 文件路径+文件名
        - Farmbeats_ImageType imageType -- 输入文件类型
        - std::string fileMD5 -- 一个输入参数为文件MD5值，可不填MD5值，视为不校验。
      
   - 返回
        - 0为成功
        - 非零详见错误码定义
   - 说明
        - 同步，且当前版本不支持取消。
        - 每次上传一个文件。
        - 支持文件校验。
        - 上传到云端的image格式是为TIFF或PNG。
        - 如果是imageType为TIFF，则直接上传。非TIFF文件，先转换成PNG文件并压缩直至小于100M，再上传。

### 接口版本
```C++
std::string Farmbeatsapi::FBGetAPIVersion()
{
    
}

```
   - 输入 
        - fbapiversion 用于存储返回内容;
   - 返回
        - 0为成功
        - 非零详见错误码定义
   - 说明
        - 同步，且当前版本不支持取消。
        - 获取API版本信息。

### 错误码定义
```C++

#define     FarmbeatsError_Success          0   
#define     FarmbeatsError_UnknowError      -1  
#define     FarmbeatsError_AuthenticationError -2 
#define     FarmbeatsError_CommunicationError -3     
#define     FarmbeatsError_UrlError           -4   
#define     FarmbeatsError_ParamsParseError   -5   
#define     FarmbeatsError_ParamsError        -6  
#define     FarmbeatsError_JsonParseError   -7   
#define     FarmbeatsError_OutOfMemory   -8    
#define     FarmbeatsError_ExtendedTypeListIsNULL   -9    
#define     FarmbeatsError_ExtendedTypeMismatch   -10    
#define		FarmbeatsError_ReadfileFailed	-11			
#define		FarmbeatsError_UploadfileFailed	-12			
#define		FarmbeatsError_RequestTimeOut	-13		

 //开发完成后会继续补充完整
```
### 请求超时时间设定

```c++
#define RESOLVE_TIMEOUT 30000
#define CONNECT_TIMEOUT 30000
#define SEND_TIMEOUT	30000
#define RECEIVE_TIMEOUT 30000

#define UPLOADFILE_RESOLVE_TIMEOUT 30000
#define UPLOADFILE_CONNECT_TIMEOUT 30000
#define UPLOADFILE_SEND_TIMEOUT	30000
#define UPLOADFILE_RECEIVE_TIMEOUT 30000
```

## 日志
     - 当前版本暂不支持
## 其他
    - 提供源码
    - API为参考设计。