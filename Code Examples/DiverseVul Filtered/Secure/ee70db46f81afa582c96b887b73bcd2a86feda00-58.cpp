		void CWebServer::RType_TransferDevice(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string sidx = request::findValue(&req, "idx");
			if (sidx.empty())
				return;

			std::string newidx = request::findValue(&req, "newidx");
			if (newidx.empty())
				return;

			std::vector<std::vector<std::string> > result;

			//Check which device is newer

			time_t now = mytime(NULL);
			struct tm tm1;
			localtime_r(&now, &tm1);
			struct tm LastUpdateTime_A;
			struct tm LastUpdateTime_B;

			result = m_sql.safe_query(
				"SELECT A.LastUpdate, B.LastUpdate FROM DeviceStatus as A, DeviceStatus as B WHERE (A.ID == '%q') AND (B.ID == '%q')",
				sidx.c_str(), newidx.c_str());
			if (result.empty())
				return;

			std::string sLastUpdate_A = result[0][0];
			std::string sLastUpdate_B = result[0][1];

			time_t timeA, timeB;
			ParseSQLdatetime(timeA, LastUpdateTime_A, sLastUpdate_A, tm1.tm_isdst);
			ParseSQLdatetime(timeB, LastUpdateTime_B, sLastUpdate_B, tm1.tm_isdst);

			if (timeA < timeB)
			{
				//Swap idx with newidx
				sidx.swap(newidx);
			}

			result = m_sql.safe_query(
				"SELECT HardwareID, DeviceID, Unit, Name, Type, SubType, SignalLevel, BatteryLevel, nValue, sValue FROM DeviceStatus WHERE (ID == '%q')",
				newidx.c_str());
			if (result.empty())
				return;

			root["status"] = "OK";
			root["title"] = "TransferDevice";

			//transfer device logs (new to old)
			m_sql.TransferDevice(newidx, sidx);

			//now delete the NEW device
			m_sql.DeleteDevices(newidx);

			m_mainworker.m_scheduler.ReloadSchedules();
		}