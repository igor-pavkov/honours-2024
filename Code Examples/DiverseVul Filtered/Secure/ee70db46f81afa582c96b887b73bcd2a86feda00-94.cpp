		void CWebServer::RType_SetUsed(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			std::string deviceid = request::findValue(&req, "deviceid");
			std::string name = request::findValue(&req, "name");
			std::string description = request::findValue(&req, "description");
			std::string sused = request::findValue(&req, "used");
			std::string sswitchtype = request::findValue(&req, "switchtype");
			std::string maindeviceidx = request::findValue(&req, "maindeviceidx");
			std::string addjvalue = request::findValue(&req, "addjvalue");
			std::string addjmulti = request::findValue(&req, "addjmulti");
			std::string addjvalue2 = request::findValue(&req, "addjvalue2");
			std::string addjmulti2 = request::findValue(&req, "addjmulti2");
			std::string setPoint = request::findValue(&req, "setpoint");
			std::string state = request::findValue(&req, "state");
			std::string mode = request::findValue(&req, "mode");
			std::string until = request::findValue(&req, "until");
			std::string clock = request::findValue(&req, "clock");
			std::string tmode = request::findValue(&req, "tmode");
			std::string fmode = request::findValue(&req, "fmode");
			std::string sCustomImage = request::findValue(&req, "customimage");

			std::string strunit = request::findValue(&req, "unit");
			std::string strParam1 = base64_decode(request::findValue(&req, "strparam1"));
			std::string strParam2 = base64_decode(request::findValue(&req, "strparam2"));
			std::string tmpstr = request::findValue(&req, "protected");
			bool bHasstrParam1 = request::hasValue(&req, "strparam1");
			int iProtected = (tmpstr == "true") ? 1 : 0;

			std::string sOptions = base64_decode(request::findValue(&req, "options"));
			std::string devoptions = CURLEncode::URLDecode(request::findValue(&req, "devoptions"));
			std::string EnergyMeterMode = CURLEncode::URLDecode(request::findValue(&req, "EnergyMeterMode"));

			char szTmp[200];

			bool bHaveUser = (session.username != "");
			int iUser = -1;
			if (bHaveUser)
			{
				iUser = FindUser(session.username.c_str());
			}

			int switchtype = -1;
			if (sswitchtype != "")
				switchtype = atoi(sswitchtype.c_str());

			if ((idx.empty()) || (sused.empty()))
				return;
			int used = (sused == "true") ? 1 : 0;
			if (maindeviceidx != "")
				used = 0;

			int CustomImage = 0;
			if (sCustomImage != "")
				CustomImage = atoi(sCustomImage.c_str());

			//Strip trailing spaces in 'name'
			name = stdstring_trim(name);

			//Strip trailing spaces in 'description'
			description = stdstring_trim(description);

			std::vector<std::vector<std::string> > result;

			result = m_sql.safe_query("SELECT Type,SubType,HardwareID FROM DeviceStatus WHERE (ID == '%q')", idx.c_str());
			if (result.empty())
				return;
			std::vector<std::string> sd = result[0];

			unsigned char dType = atoi(sd[0].c_str());
			//unsigned char dSubType=atoi(sd[1].c_str());
			int HwdID = atoi(sd[2].c_str());
			std::string sHwdID = sd[2];

			if (setPoint != "" || state != "")
			{
				double tempcelcius = atof(setPoint.c_str());
				if (m_sql.m_tempunit == TEMPUNIT_F)
				{
					//Convert back to Celsius
					tempcelcius = ConvertToCelsius(tempcelcius);
				}
				sprintf(szTmp, "%.2f", tempcelcius);

				if (dType != pTypeEvohomeZone && dType != pTypeEvohomeWater)//sql update now done in setsetpoint for evohome devices
				{
					m_sql.safe_query("UPDATE DeviceStatus SET Used=%d, sValue='%q' WHERE (ID == '%q')",
						used, szTmp, idx.c_str());
				}
			}
			if (name.empty())
			{
				m_sql.safe_query("UPDATE DeviceStatus SET Used=%d WHERE (ID == '%q')",
					used, idx.c_str());
			}
			else
			{
				if (switchtype == -1)
				{
					m_sql.safe_query("UPDATE DeviceStatus SET Used=%d, Name='%q', Description='%q' WHERE (ID == '%q')",
						used, name.c_str(), description.c_str(), idx.c_str());
				}
				else
				{
					m_sql.safe_query(
						"UPDATE DeviceStatus SET Used=%d, Name='%q', Description='%q', SwitchType=%d, CustomImage=%d WHERE (ID == '%q')",
						used, name.c_str(), description.c_str(), switchtype, CustomImage, idx.c_str());
				}
			}

			if (bHasstrParam1)
			{
				m_sql.safe_query("UPDATE DeviceStatus SET StrParam1='%q', StrParam2='%q' WHERE (ID == '%q')",
					strParam1.c_str(), strParam2.c_str(), idx.c_str());
			}

			m_sql.safe_query("UPDATE DeviceStatus SET Protected=%d WHERE (ID == '%q')", iProtected, idx.c_str());

			if (!setPoint.empty() || !state.empty())
			{
				int urights = 3;
				if (bHaveUser)
				{
					int iUser = FindUser(session.username.c_str());
					if (iUser != -1)
					{
						urights = static_cast<int>(m_users[iUser].userrights);
						_log.Log(LOG_STATUS, "User: %s initiated a SetPoint command", m_users[iUser].Username.c_str());
					}
				}
				if (urights < 1)
					return;
				if (dType == pTypeEvohomeWater)
					m_mainworker.SetSetPoint(idx, (state == "On") ? 1.0f : 0.0f, mode, until);//FIXME float not guaranteed precise?
				else if (dType == pTypeEvohomeZone)
					m_mainworker.SetSetPoint(idx, static_cast<float>(atof(setPoint.c_str())), mode, until);
				else
					m_mainworker.SetSetPoint(idx, static_cast<float>(atof(setPoint.c_str())));
			}
			else if (!clock.empty())
			{
				int urights = 3;
				if (bHaveUser)
				{
					int iUser = FindUser(session.username.c_str());
					if (iUser != -1)
					{
						urights = static_cast<int>(m_users[iUser].userrights);
						_log.Log(LOG_STATUS, "User: %s initiated a SetClock command", m_users[iUser].Username.c_str());
					}
				}
				if (urights < 1)
					return;
				m_mainworker.SetClock(idx, clock);
			}
			else if (!tmode.empty())
			{
				int urights = 3;
				if (bHaveUser)
				{
					int iUser = FindUser(session.username.c_str());
					if (iUser != -1)
					{
						urights = static_cast<int>(m_users[iUser].userrights);
						_log.Log(LOG_STATUS, "User: %s initiated a Thermostat Mode command", m_users[iUser].Username.c_str());
					}
				}
				if (urights < 1)
					return;
				m_mainworker.SetZWaveThermostatMode(idx, atoi(tmode.c_str()));
			}
			else if (!fmode.empty())
			{
				int urights = 3;
				if (bHaveUser)
				{
					int iUser = FindUser(session.username.c_str());
					if (iUser != -1)
					{
						urights = static_cast<int>(m_users[iUser].userrights);
						_log.Log(LOG_STATUS, "User: %s initiated a Thermostat Fan Mode command", m_users[iUser].Username.c_str());
					}
				}
				if (urights < 1)
					return;
				m_mainworker.SetZWaveThermostatFanMode(idx, atoi(fmode.c_str()));
			}

			if (!strunit.empty())
			{
				bool bUpdateUnit = true;
#ifdef ENABLE_PYTHON
				//check if HW is plugin
				std::vector<std::vector<std::string> > result;
				result = m_sql.safe_query("SELECT Type FROM Hardware WHERE (ID == %d)", HwdID);
				if (!result.empty())
				{
					std::vector<std::string> sd = result[0];
					_eHardwareTypes Type = (_eHardwareTypes)atoi(sd[0].c_str());
					if (Type == HTYPE_PythonPlugin)
					{
						bUpdateUnit = false;
						_log.Log(LOG_ERROR, "CWebServer::RType_SetUsed: Not allowed to change unit of device owned by plugin %u!", HwdID);
					}
				}
#endif
				if (bUpdateUnit)
				{
					m_sql.safe_query("UPDATE DeviceStatus SET Unit='%q' WHERE (ID == '%q')",
						strunit.c_str(), idx.c_str());
				}
			}
			//FIXME evohome ...we need the zone id to update the correct zone...but this should be ok as a generic call?
			if (!deviceid.empty())
			{
				m_sql.safe_query("UPDATE DeviceStatus SET DeviceID='%q' WHERE (ID == '%q')",
					deviceid.c_str(), idx.c_str());
			}
			if (!addjvalue.empty())
			{
				double faddjvalue = atof(addjvalue.c_str());
				m_sql.safe_query("UPDATE DeviceStatus SET AddjValue=%f WHERE (ID == '%q')",
					faddjvalue, idx.c_str());
			}
			if (!addjmulti.empty())
			{
				double faddjmulti = atof(addjmulti.c_str());
				if (faddjmulti == 0)
					faddjmulti = 1;
				m_sql.safe_query("UPDATE DeviceStatus SET AddjMulti=%f WHERE (ID == '%q')",
					faddjmulti, idx.c_str());
			}
			if (!addjvalue2.empty())
			{
				double faddjvalue2 = atof(addjvalue2.c_str());
				m_sql.safe_query("UPDATE DeviceStatus SET AddjValue2=%f WHERE (ID == '%q')",
					faddjvalue2, idx.c_str());
			}
			if (!addjmulti2.empty())
			{
				double faddjmulti2 = atof(addjmulti2.c_str());
				if (faddjmulti2 == 0)
					faddjmulti2 = 1;
				m_sql.safe_query("UPDATE DeviceStatus SET AddjMulti2=%f WHERE (ID == '%q')",
					faddjmulti2, idx.c_str());
			}
			if (!EnergyMeterMode.empty())
			{
				auto options = m_sql.GetDeviceOptions(idx);
				options["EnergyMeterMode"] = EnergyMeterMode;
				uint64_t ullidx = std::strtoull(idx.c_str(), nullptr, 10);
				m_sql.SetDeviceOptions(ullidx, options);
			}

			if (!devoptions.empty())
			{
				m_sql.safe_query("UPDATE DeviceStatus SET Options='%q' WHERE (ID == '%q')", devoptions.c_str(), idx.c_str());
			}

			if (used == 0)
			{
				bool bRemoveSubDevices = (request::findValue(&req, "RemoveSubDevices") == "true");

				if (bRemoveSubDevices)
				{
					//if this device was a slave device, remove it
					m_sql.safe_query("DELETE FROM LightSubDevices WHERE (DeviceRowID == '%q')", idx.c_str());
				}
				m_sql.safe_query("DELETE FROM LightSubDevices WHERE (ParentID == '%q')", idx.c_str());

				m_sql.safe_query("DELETE FROM Timers WHERE (DeviceRowID == '%q')", idx.c_str());
			}

			// Save device options
			if (!sOptions.empty())
			{
				uint64_t ullidx = std::strtoull(idx.c_str(), nullptr, 10);
				m_sql.SetDeviceOptions(ullidx, m_sql.BuildDeviceOptions(sOptions, false));
			}

			if (maindeviceidx != "")
			{
				if (maindeviceidx != idx)
				{
					//this is a sub device for another light/switch
					//first check if it is not already a sub device
					result = m_sql.safe_query("SELECT ID FROM LightSubDevices WHERE (DeviceRowID=='%q') AND (ParentID =='%q')",
						idx.c_str(), maindeviceidx.c_str());
					if (result.empty())
					{
						//no it is not, add it
						m_sql.safe_query(
							"INSERT INTO LightSubDevices (DeviceRowID, ParentID) VALUES ('%q','%q')",
							idx.c_str(),
							maindeviceidx.c_str()
						);
					}
				}
			}
			if ((used == 0) && (maindeviceidx.empty()))
			{
				//really remove it, including log etc
				m_sql.DeleteDevices(idx);
			}
			else
			{
#ifdef ENABLE_PYTHON
				// Notify plugin framework about the change
				m_mainworker.m_pluginsystem.DeviceModified(atoi(idx.c_str()));
#endif
			}
			if (!result.empty())
			{
				root["status"] = "OK";
				root["title"] = "SetUsed";
			}
			if (m_sql.m_bEnableEventSystem)
				m_mainworker.m_eventsystem.GetCurrentStates();
		}