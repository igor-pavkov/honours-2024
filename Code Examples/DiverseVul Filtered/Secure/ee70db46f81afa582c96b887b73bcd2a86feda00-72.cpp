		void CWebServer::Cmd_GetUptime(WebEmSession & session, const request& req, Json::Value &root)
		{
			//this is used in the about page, we are going to round the seconds a bit to display nicer
			time_t atime = mytime(NULL);
			time_t tuptime = atime - m_StartTime;
			//round to 5 seconds (nicer in about page)
			tuptime = ((tuptime / 5) * 5) + 5;
			int days, hours, minutes, seconds;
			days = (int)(tuptime / 86400);
			tuptime -= (days * 86400);
			hours = (int)(tuptime / 3600);
			tuptime -= (hours * 3600);
			minutes = (int)(tuptime / 60);
			tuptime -= (minutes * 60);
			seconds = (int)tuptime;
			root["status"] = "OK";
			root["title"] = "GetUptime";
			root["days"] = days;
			root["hours"] = hours;
			root["minutes"] = minutes;
			root["seconds"] = seconds;
		}