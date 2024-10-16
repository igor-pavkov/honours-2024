		void CWebServer::Cmd_GetHardwareTypes(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			root["status"] = "OK";
			root["title"] = "GetHardwareTypes";
			std::map<std::string, int> _htypes;
			for (int ii = 0; ii < HTYPE_END; ii++)
			{
				bool bDoAdd = true;
#ifndef _DEBUG
#ifdef WIN32
				if (
					(ii == HTYPE_RaspberryBMP085) ||
					(ii == HTYPE_RaspberryHTU21D) ||
					(ii == HTYPE_RaspberryTSL2561) ||
					(ii == HTYPE_RaspberryPCF8574) ||
					(ii == HTYPE_RaspberryBME280) ||
					(ii == HTYPE_RaspberryMCP23017)
					)
				{
					bDoAdd = false;
				}
				else
				{
#ifndef WITH_LIBUSB
					if (
						(ii == HTYPE_VOLCRAFTCO20) ||
						(ii == HTYPE_TE923)
						)
					{
						bDoAdd = false;
					}
#endif

		}
#endif
#endif
#ifndef WITH_OPENZWAVE
				if (ii == HTYPE_OpenZWave)
					bDoAdd = false;
#endif
#ifndef WITH_GPIO
				if (ii == HTYPE_RaspberryGPIO)
				{
					bDoAdd = false;
				}

				if (ii == HTYPE_SysfsGpio)
				{
					bDoAdd = false;
				}
#endif
				if (ii == HTYPE_PythonPlugin)
					bDoAdd = false;
				if (bDoAdd)
					_htypes[Hardware_Type_Desc(ii)] = ii;
	}
			//return a sorted hardware list
			int ii = 0;
			for (const auto & itt : _htypes)
			{
				root["result"][ii]["idx"] = itt.second;
				root["result"][ii]["name"] = itt.first;
				ii++;
			}

#ifdef ENABLE_PYTHON
			// Append Plugin list as well
			PluginList(root["result"]);
#endif
}