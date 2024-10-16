		void CWebServer::RType_LightLog(WebEmSession & session, const request& req, Json::Value &root)
		{
			uint64_t idx = 0;
			if (request::findValue(&req, "idx") != "")
			{
				idx = std::strtoull(request::findValue(&req, "idx").c_str(), nullptr, 10);
			}
			std::vector<std::vector<std::string> > result;
			//First get Device Type/SubType
			result = m_sql.safe_query("SELECT Type, SubType, SwitchType, Options FROM DeviceStatus WHERE (ID == %" PRIu64 ")",
				idx);
			if (result.empty())
				return;

			unsigned char dType = atoi(result[0][0].c_str());
			unsigned char dSubType = atoi(result[0][1].c_str());
			_eSwitchType switchtype = (_eSwitchType)atoi(result[0][2].c_str());
			std::map<std::string, std::string> options = m_sql.BuildDeviceOptions(result[0][3].c_str());

			if (
				(dType != pTypeLighting1) &&
				(dType != pTypeLighting2) &&
				(dType != pTypeLighting3) &&
				(dType != pTypeLighting4) &&
				(dType != pTypeLighting5) &&
				(dType != pTypeLighting6) &&
				(dType != pTypeFan) &&
				(dType != pTypeColorSwitch) &&
				(dType != pTypeSecurity1) &&
				(dType != pTypeSecurity2) &&
				(dType != pTypeEvohome) &&
				(dType != pTypeEvohomeRelay) &&
				(dType != pTypeCurtain) &&
				(dType != pTypeBlinds) &&
				(dType != pTypeRFY) &&
				(dType != pTypeRego6XXValue) &&
				(dType != pTypeChime) &&
				(dType != pTypeThermostat2) &&
				(dType != pTypeThermostat3) &&
				(dType != pTypeThermostat4) &&
				(dType != pTypeRemote) &&
				(dType != pTypeGeneralSwitch) &&
				(dType != pTypeHomeConfort) &&
				(dType != pTypeFS20) &&
				(!((dType == pTypeRadiator1) && (dSubType == sTypeSmartwaresSwitchRadiator)))
				)
				return; //no light device! we should not be here!

			root["status"] = "OK";
			root["title"] = "LightLog";

			result = m_sql.safe_query("SELECT ROWID, nValue, sValue, Date FROM LightingLog WHERE (DeviceRowID==%" PRIu64 ") ORDER BY Date DESC", idx);
			if (!result.empty())
			{
				std::map<std::string, std::string> selectorStatuses;
				if (switchtype == STYPE_Selector) {
					GetSelectorSwitchStatuses(options, selectorStatuses);
				}

				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					int nValue = atoi(sd[1].c_str());
					std::string sValue = sd[2];

					//skip 0-values in log for MediaPlayers
					if ((switchtype == STYPE_Media) && (sValue == "0")) continue;

					root["result"][ii]["idx"] = sd[0];

					//add light details
					std::string lstatus = "";
					std::string ldata = "";
					int llevel = 0;
					bool bHaveDimmer = false;
					bool bHaveSelector = false;
					bool bHaveGroupCmd = false;
					int maxDimLevel = 0;

					if (switchtype == STYPE_Media) {
						lstatus = sValue;
						ldata = lstatus;

					}
					else if (switchtype == STYPE_Selector)
					{
						if (ii == 0) {
							bHaveSelector = true;
							maxDimLevel = selectorStatuses.size();
						}
						if (!selectorStatuses.empty()) {

							std::string sLevel = selectorStatuses[sValue];
							ldata = sLevel;
							lstatus = "Set Level: " + sLevel;
							llevel = atoi(sValue.c_str());
						}
					}
					else {
						GetLightStatus(dType, dSubType, switchtype, nValue, sValue, lstatus, llevel, bHaveDimmer, maxDimLevel, bHaveGroupCmd);
						ldata = lstatus;
					}

					if (ii == 0)
					{
						root["HaveDimmer"] = bHaveDimmer;
						root["result"][ii]["MaxDimLevel"] = maxDimLevel;
						root["HaveGroupCmd"] = bHaveGroupCmd;
						root["HaveSelector"] = bHaveSelector;
					}

					root["result"][ii]["Date"] = sd[3];
					root["result"][ii]["Data"] = ldata;
					root["result"][ii]["Status"] = lstatus;
					root["result"][ii]["Level"] = llevel;

					ii++;
				}
			}
		}