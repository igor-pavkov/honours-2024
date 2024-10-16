		void CWebServer::Cmd_GetLog(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetLog";

			time_t lastlogtime = 0;
			std::string slastlogtime = request::findValue(&req, "lastlogtime");
			if (slastlogtime != "")
			{
				std::stringstream s_str(slastlogtime);
				s_str >> lastlogtime;
			}

			_eLogLevel lLevel = LOG_NORM;
			std::string sloglevel = request::findValue(&req, "loglevel");
			if (!sloglevel.empty())
			{
				lLevel = (_eLogLevel)atoi(sloglevel.c_str());
			}

			std::list<CLogger::_tLogLineStruct> logmessages = _log.GetLog(lLevel);
			int ii = 0;
			for (const auto & itt : logmessages)
			{
				if (itt.logtime > lastlogtime)
				{
					std::stringstream szLogTime;
					szLogTime << itt.logtime;
					root["LastLogTime"] = szLogTime.str();
					root["result"][ii]["level"] = static_cast<int>(itt.level);
					root["result"][ii]["message"] = itt.logmessage;
					ii++;
				}
			}
		}