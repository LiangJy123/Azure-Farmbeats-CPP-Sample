# FarmbeatsAPI 
----
### 目录
- [Farmbeatsapi](./Farmbeatsapi) - DLL工程
- [FarmbeatsapiTest](./Farmbeatsapi) - 调用DLL的示例工程
- [APIDesign.md](APIDesign.md) - 参考设计说明
### 准备
```
string fbhub_url = "https://xxxxxxxx.azurewebsites.net";
string clientID = "a925xxxx-xxxx-xxxx-xxxx-xxxxf9b4xxxx";
string clientSecret = "xxxxxxxxxxxxxxxxxxx";
string tenantID = "xxxx0275-xxxx-xxxx-xxxx-xxxx0dafxxxx";
string farmid = "xxxxcafc-xxxx-xxxx-xxxx-xxxx08c5xxxx";
```
### 参考
- Farmbeats websites swagger (REST API)
- AAD REST API
### Lisence
- Json解析库使用的Tencent开源rapidjson项目，需要遵守MIT开源协议。
  - 详见 https://github.com/Tencent/rapidjson
