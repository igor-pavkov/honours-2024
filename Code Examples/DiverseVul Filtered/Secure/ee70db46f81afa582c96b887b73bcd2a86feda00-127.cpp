		void CWebServer::Cmd_GetSerialDevices(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetSerialDevices";

			bool bUseDirectPath = false;
			std::vector<std::string> serialports = GetSerialPorts(bUseDirectPath);
			int ii = 0;
			for (const auto & itt : serialports)
			{
				root["result"][ii]["name"] = itt;
				root["result"][ii]["value"] = ii;
				ii++;
			}
		}