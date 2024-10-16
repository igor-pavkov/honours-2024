		void CWebServer::Cmd_GetConfig(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights == -1)
			{
				session.reply_status = reply::forbidden;
				return;//Only auth user allowed
			}

			root["status"] = "OK";
			root["title"] = "GetConfig";

			bool bHaveUser = (session.username != "");
			int urights = 3;
			unsigned long UserID = 0;
			if (bHaveUser)
			{
				int iUser = FindUser(session.username.c_str());
				if (iUser != -1)
				{
					urights = static_cast<int>(m_users[iUser].userrights);
					UserID = m_users[iUser].ID;
				}

			}

			int nValue;
			std::string sValue;

			if (m_sql.GetPreferencesVar("Language", sValue))
			{
				root["language"] = sValue;
			}
			if (m_sql.GetPreferencesVar("DegreeDaysBaseTemperature", sValue))
			{
				root["DegreeDaysBaseTemperature"] = atof(sValue.c_str());
			}

			nValue = 0;
			int iDashboardType = 0;
			m_sql.GetPreferencesVar("DashboardType", iDashboardType);
			root["DashboardType"] = iDashboardType;
			m_sql.GetPreferencesVar("MobileType", nValue);
			root["MobileType"] = nValue;

			nValue = 1;
			m_sql.GetPreferencesVar("5MinuteHistoryDays", nValue);
			root["FiveMinuteHistoryDays"] = nValue;

			nValue = 1;
			m_sql.GetPreferencesVar("ShowUpdateEffect", nValue);
			root["result"]["ShowUpdatedEffect"] = (nValue == 1);

			root["AllowWidgetOrdering"] = m_sql.m_bAllowWidgetOrdering;

			root["WindScale"] = m_sql.m_windscale*10.0f;
			root["WindSign"] = m_sql.m_windsign;
			root["TempScale"] = m_sql.m_tempscale;
			root["TempSign"] = m_sql.m_tempsign;

			std::string Latitude = "1";
			std::string Longitude = "1";
			if (m_sql.GetPreferencesVar("Location", nValue, sValue))
			{
				std::vector<std::string> strarray;
				StringSplit(sValue, ";", strarray);

				if (strarray.size() == 2)
				{
					Latitude = strarray[0];
					Longitude = strarray[1];
				}
			}
			root["Latitude"] = Latitude;
			root["Longitude"] = Longitude;

#ifndef NOCLOUD
			bool bEnableTabProxy = request::get_req_header(&req, "X-From-MyDomoticz") != NULL;
#else
			bool bEnableTabProxy = false;
#endif
			int bEnableTabDashboard = 1;
			int bEnableTabFloorplans = 1;
			int bEnableTabLight = 1;
			int bEnableTabScenes = 1;
			int bEnableTabTemp = 1;
			int bEnableTabWeather = 1;
			int bEnableTabUtility = 1;
			int bEnableTabCustom = 1;

			std::vector<std::vector<std::string> > result;

			if ((UserID != 0) && (UserID != 10000))
			{
				result = m_sql.safe_query("SELECT TabsEnabled FROM Users WHERE (ID==%lu)",
					UserID);
				if (!result.empty())
				{
					int TabsEnabled = atoi(result[0][0].c_str());
					bEnableTabLight = (TabsEnabled&(1 << 0));
					bEnableTabScenes = (TabsEnabled&(1 << 1));
					bEnableTabTemp = (TabsEnabled&(1 << 2));
					bEnableTabWeather = (TabsEnabled&(1 << 3));
					bEnableTabUtility = (TabsEnabled&(1 << 4));
					bEnableTabCustom = (TabsEnabled&(1 << 5));
					bEnableTabFloorplans = (TabsEnabled&(1 << 6));
				}
			}
			else
			{
				m_sql.GetPreferencesVar("EnableTabFloorplans", bEnableTabFloorplans);
				m_sql.GetPreferencesVar("EnableTabLights", bEnableTabLight);
				m_sql.GetPreferencesVar("EnableTabScenes", bEnableTabScenes);
				m_sql.GetPreferencesVar("EnableTabTemp", bEnableTabTemp);
				m_sql.GetPreferencesVar("EnableTabWeather", bEnableTabWeather);
				m_sql.GetPreferencesVar("EnableTabUtility", bEnableTabUtility);
				m_sql.GetPreferencesVar("EnableTabCustom", bEnableTabCustom);
			}
			if (iDashboardType == 3)
			{
				//Floorplan , no need to show a tab floorplan
				bEnableTabFloorplans = 0;
			}
			root["result"]["EnableTabProxy"] = bEnableTabProxy;
			root["result"]["EnableTabDashboard"] = bEnableTabDashboard != 0;
			root["result"]["EnableTabFloorplans"] = bEnableTabFloorplans != 0;
			root["result"]["EnableTabLights"] = bEnableTabLight != 0;
			root["result"]["EnableTabScenes"] = bEnableTabScenes != 0;
			root["result"]["EnableTabTemp"] = bEnableTabTemp != 0;
			root["result"]["EnableTabWeather"] = bEnableTabWeather != 0;
			root["result"]["EnableTabUtility"] = bEnableTabUtility != 0;
			root["result"]["EnableTabCustom"] = bEnableTabCustom != 0;

			if (bEnableTabCustom)
			{
				//Add custom templates
				DIR *lDir;
				struct dirent *ent;
				std::string templatesFolder = szWWWFolder + "/templates";
				int iFile = 0;
				if ((lDir = opendir(templatesFolder.c_str())) != NULL)
				{
					while ((ent = readdir(lDir)) != NULL)
					{
						std::string filename = ent->d_name;
						size_t pos = filename.find(".htm");
						if (pos != std::string::npos)
						{
							std::string shortfile = filename.substr(0, pos);
							root["result"]["templates"][iFile++] = shortfile;
						}
					}
					closedir(lDir);
				}
			}
		}