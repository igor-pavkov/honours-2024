		void CWebServer::Cmd_GetUnusedPlanDevices(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetUnusedPlanDevices";
			std::string sunique = request::findValue(&req, "unique");
			if (sunique.empty())
				return;
			int iUnique = (sunique == "true") ? 1 : 0;
			int ii = 0;

			std::vector<std::vector<std::string> > result;
			std::vector<std::vector<std::string> > result2;
			result = m_sql.safe_query("SELECT T1.[ID], T1.[Name], T1.[Type], T1.[SubType], T2.[Name] AS HardwareName FROM DeviceStatus as T1, Hardware as T2 WHERE (T1.[Used]==1) AND (T2.[ID]==T1.[HardwareID]) ORDER BY T2.[Name], T1.[Name]");
			if (!result.empty())
			{
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					bool bDoAdd = true;
					if (iUnique)
					{
						result2 = m_sql.safe_query("SELECT ID FROM DeviceToPlansMap WHERE (DeviceRowID=='%q') AND (DevSceneType==0)",
							sd[0].c_str());
						bDoAdd = (result2.size() == 0);
					}
					if (bDoAdd)
					{
						int _dtype = atoi(sd[2].c_str());
						std::string Name = "[" + sd[4] + "] " + sd[1] + " (" + RFX_Type_Desc(_dtype, 1) + "/" + RFX_Type_SubType_Desc(_dtype, atoi(sd[3].c_str())) + ")";
						root["result"][ii]["type"] = 0;
						root["result"][ii]["idx"] = sd[0];
						root["result"][ii]["Name"] = Name;
						ii++;
					}
				}
			}
			//Add Scenes
			result = m_sql.safe_query("SELECT ID, Name FROM Scenes ORDER BY Name");
			if (!result.empty())
			{
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					bool bDoAdd = true;
					if (iUnique)
					{
						result2 = m_sql.safe_query("SELECT ID FROM DeviceToPlansMap WHERE (DeviceRowID=='%q') AND (DevSceneType==1)",
							sd[0].c_str());
						bDoAdd = (result2.size() == 0);
					}
					if (bDoAdd)
					{
						root["result"][ii]["type"] = 1;
						root["result"][ii]["idx"] = sd[0];
						std::string sname = "[Scene] " + sd[1];
						root["result"][ii]["Name"] = sname;
						ii++;
					}
				}
			}
		}