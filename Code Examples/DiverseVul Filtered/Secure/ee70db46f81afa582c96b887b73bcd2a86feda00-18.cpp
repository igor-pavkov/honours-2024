		void CWebServer::Cmd_GetNewHistory(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetNewHistory";

			std::string historyfile;
			int nValue;
			m_sql.GetPreferencesVar("ReleaseChannel", nValue);
			bool bIsBetaChannel = (nValue != 0);

			std::string szHistoryURL = "https://www.domoticz.com/download.php?channel=stable&type=history";
			if (bIsBetaChannel)
			{
				utsname my_uname;
				if (uname(&my_uname) < 0)
					return;

				std::string systemname = my_uname.sysname;
				std::string machine = my_uname.machine;
				std::transform(systemname.begin(), systemname.end(), systemname.begin(), ::tolower);

				if (machine == "armv6l")
				{
					//Seems like old arm systems can also use the new arm build
					machine = "armv7l";
				}

				if (((machine != "armv6l") && (machine != "armv7l") && (systemname != "windows") && (machine != "x86_64") && (machine != "aarch64")) || (strstr(my_uname.release, "ARCH+") != NULL))
					szHistoryURL = "https://www.domoticz.com/download.php?channel=beta&type=history";
				else
					szHistoryURL = "https://www.domoticz.com/download.php?channel=beta&type=history&system=" + systemname + "&machine=" + machine;
			}
			if (!HTTPClient::GET(szHistoryURL, historyfile))
			{
				historyfile = "Unable to get Online History document !!";
			}

			std::istringstream stream(historyfile);
			std::string sLine;
			int ii = 0;
			while (std::getline(stream, sLine))
			{
				root["LastLogTime"] = "";
				if (sLine.find("Version ") == 0)
					root["result"][ii]["level"] = 1;
				else
					root["result"][ii]["level"] = 0;
				root["result"][ii]["message"] = sLine;
				ii++;
			}
		}