		void CWebServer::Cmd_SetPlanDeviceCoords(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string idx = request::findValue(&req, "idx");
			std::string planidx = request::findValue(&req, "planidx");
			std::string xoffset = request::findValue(&req, "xoffset");
			std::string yoffset = request::findValue(&req, "yoffset");
			std::string type = request::findValue(&req, "DevSceneType");
			if ((idx.empty()) || (planidx.empty()) || (xoffset.empty()) || (yoffset.empty()))
				return;
			if (type != "1") type = "0";  // 0 = Device, 1 = Scene/Group
			root["status"] = "OK";
			root["title"] = "SetPlanDeviceCoords";
			m_sql.safe_query("UPDATE DeviceToPlansMap SET [XOffset] = '%q', [YOffset] = '%q' WHERE (DeviceRowID='%q') and (PlanID='%q') and (DevSceneType='%q')",
				xoffset.c_str(), yoffset.c_str(), idx.c_str(), planidx.c_str(), type.c_str());
			_log.Log(LOG_STATUS, "(Floorplan) Device '%s' coordinates set to '%s,%s' in plan '%s'.", idx.c_str(), xoffset.c_str(), yoffset.c_str(), planidx.c_str());
		}