		void CWebServer::RType_Hardware(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "Hardware";

#ifdef WITH_OPENZWAVE
			m_ZW_Hwidx = -1;
#endif

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID, Name, Enabled, Type, Address, Port, SerialPort, Username, Password, Extra, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6, DataTimeout FROM Hardware ORDER BY ID ASC");
			if (!result.empty())
			{
				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					_eHardwareTypes hType = (_eHardwareTypes)atoi(sd[3].c_str());
					if (hType == HTYPE_DomoticzInternal)
						continue;
					if (hType == HTYPE_RESERVED_FOR_YOU_1)
						continue;
					root["result"][ii]["idx"] = sd[0];
					root["result"][ii]["Name"] = sd[1];
					root["result"][ii]["Enabled"] = (sd[2] == "1") ? "true" : "false";
					root["result"][ii]["Type"] = hType;
					root["result"][ii]["Address"] = sd[4];
					root["result"][ii]["Port"] = atoi(sd[5].c_str());
					root["result"][ii]["SerialPort"] = sd[6];
					root["result"][ii]["Username"] = sd[7];
					root["result"][ii]["Password"] = sd[8];
					root["result"][ii]["Extra"] = sd[9];

					if (hType == HTYPE_PythonPlugin) {
						root["result"][ii]["Mode1"] = sd[10];  // Plugins can have non-numeric values in the Mode fields
						root["result"][ii]["Mode2"] = sd[11];
						root["result"][ii]["Mode3"] = sd[12];
						root["result"][ii]["Mode4"] = sd[13];
						root["result"][ii]["Mode5"] = sd[14];
						root["result"][ii]["Mode6"] = sd[15];
					}
					else {
						root["result"][ii]["Mode1"] = atoi(sd[10].c_str());
						root["result"][ii]["Mode2"] = atoi(sd[11].c_str());
						root["result"][ii]["Mode3"] = atoi(sd[12].c_str());
						root["result"][ii]["Mode4"] = atoi(sd[13].c_str());
						root["result"][ii]["Mode5"] = atoi(sd[14].c_str());
						root["result"][ii]["Mode6"] = atoi(sd[15].c_str());
					}
					root["result"][ii]["DataTimeout"] = atoi(sd[16].c_str());

					//Special case for openzwave (status for nodes queried)
					CDomoticzHardwareBase *pHardware = m_mainworker.GetHardware(atoi(sd[0].c_str()));
					if (pHardware != NULL)
					{
						if (
							(pHardware->HwdType == HTYPE_RFXtrx315) ||
							(pHardware->HwdType == HTYPE_RFXtrx433) ||
							(pHardware->HwdType == HTYPE_RFXtrx868) ||
							(pHardware->HwdType == HTYPE_RFXLAN)
							)
						{
							CRFXBase *pMyHardware = reinterpret_cast<CRFXBase*>(pHardware);
							if (!pMyHardware->m_Version.empty())
								root["result"][ii]["version"] = pMyHardware->m_Version;
							else
								root["result"][ii]["version"] = sd[11];
							root["result"][ii]["noiselvl"] = pMyHardware->m_NoiseLevel;
						}
						else if ((pHardware->HwdType == HTYPE_MySensorsUSB) || (pHardware->HwdType == HTYPE_MySensorsTCP) || (pHardware->HwdType == HTYPE_MySensorsMQTT))
						{
							MySensorsBase *pMyHardware = reinterpret_cast<MySensorsBase*>(pHardware);
							root["result"][ii]["version"] = pMyHardware->GetGatewayVersion();
						}
						else if ((pHardware->HwdType == HTYPE_OpenThermGateway) || (pHardware->HwdType == HTYPE_OpenThermGatewayTCP))
						{
							OTGWBase *pMyHardware = reinterpret_cast<OTGWBase*>(pHardware);
							root["result"][ii]["version"] = pMyHardware->m_Version;
						}
						else if ((pHardware->HwdType == HTYPE_RFLINKUSB) || (pHardware->HwdType == HTYPE_RFLINKTCP))
						{
							CRFLinkBase *pMyHardware = reinterpret_cast<CRFLinkBase*>(pHardware);
							root["result"][ii]["version"] = pMyHardware->m_Version;
						}
						else
						{
#ifdef WITH_OPENZWAVE
							if (pHardware->HwdType == HTYPE_OpenZWave)
							{
								COpenZWave *pOZWHardware = reinterpret_cast<COpenZWave*>(pHardware);
								root["result"][ii]["version"] = pOZWHardware->GetVersionLong();
								root["result"][ii]["NodesQueried"] = (pOZWHardware->m_awakeNodesQueried || pOZWHardware->m_allNodesQueried);
							}
#endif
						}
					}
					ii++;
				}
			}
		}