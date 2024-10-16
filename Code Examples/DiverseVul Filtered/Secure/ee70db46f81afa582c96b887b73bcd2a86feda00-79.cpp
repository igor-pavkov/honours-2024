		void CWebServer::Cmd_GetCosts(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			char szTmp[100];
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT Type, SubType, nValue, sValue FROM DeviceStatus WHERE (ID=='%q')",
				idx.c_str());
			if (!result.empty())
			{
				std::vector<std::string> sd = result[0];

				int nValue = 0;
				root["status"] = "OK";
				root["title"] = "GetElectraCosts";
				m_sql.GetPreferencesVar("CostEnergy", nValue);
				root["CostEnergy"] = nValue;
				m_sql.GetPreferencesVar("CostEnergyT2", nValue);
				root["CostEnergyT2"] = nValue;
				m_sql.GetPreferencesVar("CostEnergyR1", nValue);
				root["CostEnergyR1"] = nValue;
				m_sql.GetPreferencesVar("CostEnergyR2", nValue);
				root["CostEnergyR2"] = nValue;
				m_sql.GetPreferencesVar("CostGas", nValue);
				root["CostGas"] = nValue;
				m_sql.GetPreferencesVar("CostWater", nValue);
				root["CostWater"] = nValue;

				int tValue = 1000;
				if (m_sql.GetPreferencesVar("MeterDividerWater", tValue))
				{
					root["DividerWater"] = float(tValue);
				}

				unsigned char dType = atoi(sd[0].c_str());
				//unsigned char subType = atoi(sd[1].c_str());
				//nValue = (unsigned char)atoi(sd[2].c_str());
				std::string sValue = sd[3];

				if (dType == pTypeP1Power)
				{
					//also provide the counter values

					std::vector<std::string> splitresults;
					StringSplit(sValue, ";", splitresults);
					if (splitresults.size() != 6)
						return;

					float EnergyDivider = 1000.0f;
					if (m_sql.GetPreferencesVar("MeterDividerEnergy", tValue))
					{
						EnergyDivider = float(tValue);
					}

					unsigned long long powerusage1 = std::strtoull(splitresults[0].c_str(), nullptr, 10);
					unsigned long long powerusage2 = std::strtoull(splitresults[1].c_str(), nullptr, 10);
					unsigned long long powerdeliv1 = std::strtoull(splitresults[2].c_str(), nullptr, 10);
					unsigned long long powerdeliv2 = std::strtoull(splitresults[3].c_str(), nullptr, 10);
					unsigned long long usagecurrent = std::strtoull(splitresults[4].c_str(), nullptr, 10);
					unsigned long long delivcurrent = std::strtoull(splitresults[5].c_str(), nullptr, 10);

					powerdeliv1 = (powerdeliv1 < 10) ? 0 : powerdeliv1;
					powerdeliv2 = (powerdeliv2 < 10) ? 0 : powerdeliv2;

					sprintf(szTmp, "%.03f", float(powerusage1) / EnergyDivider);
					root["CounterT1"] = szTmp;
					sprintf(szTmp, "%.03f", float(powerusage2) / EnergyDivider);
					root["CounterT2"] = szTmp;
					sprintf(szTmp, "%.03f", float(powerdeliv1) / EnergyDivider);
					root["CounterR1"] = szTmp;
					sprintf(szTmp, "%.03f", float(powerdeliv2) / EnergyDivider);
					root["CounterR2"] = szTmp;
				}
			}
		}