		void CWebServer::Cmd_GetPlanDevices(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			root["status"] = "OK";
			root["title"] = "GetPlanDevices";

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID, DevSceneType, DeviceRowID, [Order] FROM DeviceToPlansMap WHERE (PlanID=='%q') ORDER BY [Order]",
				idx.c_str());
			if (!result.empty())
			{
				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					std::string ID = sd[0];
					int DevSceneType = atoi(sd[1].c_str());
					std::string DevSceneRowID = sd[2];

					std::string Name = "";
					if (DevSceneType == 0)
					{
						std::vector<std::vector<std::string> > result2;
						result2 = m_sql.safe_query("SELECT Name FROM DeviceStatus WHERE (ID=='%q')",
							DevSceneRowID.c_str());
						if (!result2.empty())
						{
							Name = result2[0][0];
						}
					}
					else
					{
						std::vector<std::vector<std::string> > result2;
						result2 = m_sql.safe_query("SELECT Name FROM Scenes WHERE (ID=='%q')",
							DevSceneRowID.c_str());
						if (!result2.empty())
						{
							Name = "[Scene] " + result2[0][0];
						}
					}
					if (Name != "")
					{
						root["result"][ii]["idx"] = ID;
						root["result"][ii]["devidx"] = DevSceneRowID;
						root["result"][ii]["type"] = DevSceneType;
						root["result"][ii]["DevSceneRowID"] = DevSceneRowID;
						root["result"][ii]["order"] = sd[3];
						root["result"][ii]["Name"] = Name;
						ii++;
					}
				}
			}
		}