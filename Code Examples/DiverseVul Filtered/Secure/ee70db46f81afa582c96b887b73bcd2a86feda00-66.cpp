		void CWebServer::Cmd_UpdateDevice(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights < 1)
			{
				session.reply_status = reply::forbidden;
				return; //only user or higher allowed
			}

			std::string idx = request::findValue(&req, "idx");

			if (!IsIdxForUser(&session, atoi(idx.c_str())))
			{
				_log.Log(LOG_ERROR, "User: %s tried to update an Unauthorized device!", session.username.c_str());
				session.reply_status = reply::forbidden;
				return;
			}

			std::string hid = request::findValue(&req, "hid");
			std::string did = request::findValue(&req, "did");
			std::string dunit = request::findValue(&req, "dunit");
			std::string dtype = request::findValue(&req, "dtype");
			std::string dsubtype = request::findValue(&req, "dsubtype");

			std::string nvalue = request::findValue(&req, "nvalue");
			std::string svalue = request::findValue(&req, "svalue");

			if ((nvalue.empty() && svalue.empty()))
			{
				return;
			}

			int signallevel = 12;
			int batterylevel = 255;

			if (idx.empty())
			{
				//No index supplied, check if raw parameters where supplied
				if (
					(hid.empty()) ||
					(did.empty()) ||
					(dunit.empty()) ||
					(dtype.empty()) ||
					(dsubtype.empty())
					)
					return;
			}
			else
			{
				//Get the raw device parameters
				std::vector<std::vector<std::string> > result;
				result = m_sql.safe_query("SELECT HardwareID, DeviceID, Unit, Type, SubType FROM DeviceStatus WHERE (ID=='%q')",
					idx.c_str());
				if (result.empty())
					return;
				hid = result[0][0];
				did = result[0][1];
				dunit = result[0][2];
				dtype = result[0][3];
				dsubtype = result[0][4];
			}

			int HardwareID = atoi(hid.c_str());
			std::string DeviceID = did;
			int unit = atoi(dunit.c_str());
			int devType = atoi(dtype.c_str());
			int subType = atoi(dsubtype.c_str());

			uint64_t ulIdx = std::strtoull(idx.c_str(), nullptr, 10);

			int invalue = atoi(nvalue.c_str());

			std::string sSignalLevel = request::findValue(&req, "rssi");
			if (sSignalLevel != "")
			{
				signallevel = atoi(sSignalLevel.c_str());
			}
			std::string sBatteryLevel = request::findValue(&req, "battery");
			if (sBatteryLevel != "")
			{
				batterylevel = atoi(sBatteryLevel.c_str());
			}
			if (m_mainworker.UpdateDevice(HardwareID, DeviceID, unit, devType, subType, invalue, svalue, signallevel, batterylevel))
			{
				root["status"] = "OK";
				root["title"] = "Update Device";
			}
		}