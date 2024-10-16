		void CWebServer::Cmd_UpdateHardware(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			std::string name = CURLEncode::URLDecode(request::findValue(&req, "name"));
			std::string senabled = request::findValue(&req, "enabled");
			std::string shtype = request::findValue(&req, "htype");
			std::string address = request::findValue(&req, "address");
			std::string sport = request::findValue(&req, "port");
			std::string username = CURLEncode::URLDecode(request::findValue(&req, "username"));
			std::string password = CURLEncode::URLDecode(request::findValue(&req, "password"));
			std::string extra = CURLEncode::URLDecode(request::findValue(&req, "extra"));
			std::string sdatatimeout = request::findValue(&req, "datatimeout");

			if (
				(name.empty()) ||
				(senabled.empty()) ||
				(shtype.empty())
				)
				return;

			int mode1 = atoi(request::findValue(&req, "Mode1").c_str());
			int mode2 = atoi(request::findValue(&req, "Mode2").c_str());
			int mode3 = atoi(request::findValue(&req, "Mode3").c_str());
			int mode4 = atoi(request::findValue(&req, "Mode4").c_str());
			int mode5 = atoi(request::findValue(&req, "Mode5").c_str());
			int mode6 = atoi(request::findValue(&req, "Mode6").c_str());

			bool bEnabled = (senabled == "true") ? true : false;

			_eHardwareTypes htype = (_eHardwareTypes)atoi(shtype.c_str());
			int iDataTimeout = atoi(sdatatimeout.c_str());

			int port = atoi(sport.c_str());

			bool bIsSerial = false;

			if (IsSerialDevice(htype))
			{
				//USB/System
				bIsSerial = true;
				if (bEnabled)
				{
					if (sport.empty())
						return; //need to have a serial port
				}
			}
			else if (
				(htype == HTYPE_RFXLAN) || (htype == HTYPE_P1SmartMeterLAN) ||
				(htype == HTYPE_YouLess) || (htype == HTYPE_OpenThermGatewayTCP) || (htype == HTYPE_LimitlessLights) ||
				(htype == HTYPE_SolarEdgeTCP) || (htype == HTYPE_WOL) || (htype == HTYPE_S0SmartMeterTCP) || (htype == HTYPE_ECODEVICES) || (htype == HTYPE_Mochad) ||
				(htype == HTYPE_MySensorsTCP) || (htype == HTYPE_MySensorsMQTT) || (htype == HTYPE_MQTT) || (htype == HTYPE_TTN_MQTT) || (htype == HTYPE_FRITZBOX) || (htype == HTYPE_ETH8020) || (htype == HTYPE_Sterbox) ||
				(htype == HTYPE_KMTronicTCP) || (htype == HTYPE_KMTronicUDP) || (htype == HTYPE_SOLARMAXTCP) || (htype == HTYPE_RelayNet) || (htype == HTYPE_SatelIntegra) || (htype == HTYPE_eHouseTCP) || (htype == HTYPE_RFLINKTCP) ||
				(htype == HTYPE_Comm5TCP || (htype == HTYPE_Comm5SMTCP) || (htype == HTYPE_CurrentCostMeterLAN)) ||
				(htype == HTYPE_NefitEastLAN) || (htype == HTYPE_DenkoviHTTPDevices) || (htype == HTYPE_DenkoviTCPDevices) || (htype == HTYPE_Ec3kMeterTCP) || (htype == HTYPE_MultiFun) || (htype == HTYPE_ZIBLUETCP) || (htype == HTYPE_OnkyoAVTCP)
				) {
				//Lan
				if (address.empty())
					return;
			}
			else if (htype == HTYPE_DomoticzInternal) {
				// DomoticzInternal cannot be updated
				return;
			}
			else if (htype == HTYPE_Domoticz) {
				//Remote Domoticz
				if (address.empty())
					return;
			}
			else if (htype == HTYPE_System) {
				//There should be only one, and with this ID
				std::vector<std::vector<std::string> > result;
				result = m_sql.safe_query("SELECT ID FROM Hardware WHERE (Type==%d)", HTYPE_System);
				if (!result.empty())
				{
					int hID = atoi(result[0][0].c_str());
					int aID = atoi(idx.c_str());
					if (hID != aID)
						return;
				}
			}
			else if (htype == HTYPE_TE923) {
				//All fine here
			}
			else if (htype == HTYPE_VOLCRAFTCO20) {
				//All fine here
			}
			else if (htype == HTYPE_1WIRE) {
				//All fine here
			}
			else if (htype == HTYPE_Pinger) {
				//All fine here
			}
			else if (htype == HTYPE_Kodi) {
				//All fine here
			}
			else if (htype == HTYPE_PanasonicTV) {
				//All fine here
			}
			else if (htype == HTYPE_LogitechMediaServer) {
				//All fine here
			}
			else if (htype == HTYPE_RaspberryBMP085) {
				//All fine here
			}
			else if (htype == HTYPE_RaspberryHTU21D) {
				//All fine here
			}
			else if (htype == HTYPE_RaspberryTSL2561) {
				//All fine here
			}
			else if (htype == HTYPE_RaspberryBME280) {
				//All fine here
			}
			else if (htype == HTYPE_RaspberryMCP23017) {
				//all fine here!
			}
			else if (htype == HTYPE_Dummy) {
				//All fine here
			}
			else if (htype == HTYPE_EVOHOME_SCRIPT || htype == HTYPE_EVOHOME_SERIAL || htype == HTYPE_EVOHOME_WEB || htype == HTYPE_EVOHOME_TCP) {
				//All fine here
			}
			else if (htype == HTYPE_PiFace) {
				//All fine here
			}
			else if (htype == HTYPE_HTTPPOLLER) {
				//all fine here!
			}
			else if (htype == HTYPE_BleBox) {
				//All fine here
			}
			else if (htype == HTYPE_HEOS) {
				//All fine here
			}
			else if (htype == HTYPE_Yeelight) {
				//All fine here
			}
			else if (htype == HTYPE_XiaomiGateway) {
				//All fine here
			}
			else if (htype == HTYPE_Arilux) {
				//All fine here
			}
			else if (htype == HTYPE_USBtinGateway) {
				//All fine here
			}
			else if (
				(htype == HTYPE_Wunderground) ||
				(htype == HTYPE_DarkSky) ||
				(htype == HTYPE_AccuWeather) ||
				(htype == HTYPE_OpenWeatherMap) ||
				(htype == HTYPE_ICYTHERMOSTAT) ||
				(htype == HTYPE_TOONTHERMOSTAT) ||
				(htype == HTYPE_AtagOne) ||
				(htype == HTYPE_PVOUTPUT_INPUT) ||
				(htype == HTYPE_NEST) ||
				(htype == HTYPE_ANNATHERMOSTAT) ||
				(htype == HTYPE_THERMOSMART) ||
				(htype == HTYPE_Tado) ||
				(htype == HTYPE_Netatmo)
				)
			{
				if (
					(username.empty()) ||
					(password.empty())
					)
					return;
			}
			else if (htype == HTYPE_SolarEdgeAPI)
			{
				if (
					(username.empty())
					)
					return;
			}
			else if (htype == HTYPE_Nest_OAuthAPI) {
				if (
					(username == "") &&
					(extra == "||")
					)
					return;
			}
			else if (htype == HTYPE_HARMONY_HUB) {
				if (
					(address.empty())
					)
					return;
			}
			else if (htype == HTYPE_Philips_Hue) {
				if (
					(username.empty()) ||
					(address.empty())
					)
					return;
				if (port == 0)
					port = 80;
			}
			else if (htype == HTYPE_RaspberryGPIO) {
				//all fine here!
			}
			else if (htype == HTYPE_SysfsGpio) {
				//all fine here!
			}
			else if (htype == HTYPE_Rtl433) {
				//all fine here!
			}
			else if (htype == HTYPE_Daikin) {
				//all fine here!
			}
			else if (htype == HTYPE_SBFSpot) {
				if (username.empty())
					return;
			}
			else if (htype == HTYPE_WINDDELEN) {
				std::string mill_id = request::findValue(&req, "Mode1");
				if (
					(mill_id.empty()) ||
					(sport.empty())
					)
					return;
			}
			else if (htype == HTYPE_Honeywell) {
				//All fine here
			}
			else if (htype == HTYPE_OpenWebNetTCP) {
				//All fine here
			}
			else if (htype == HTYPE_PythonPlugin) {
				//All fine here
			}
			else if (htype == HTYPE_GoodweAPI) {
				if (username.empty()) {
					return;
				}
			}
			else if (htype == HTYPE_RaspberryPCF8574) {
				//All fine here
			}
			else if (htype == HTYPE_OpenWebNetUSB) {
				//All fine here
			}
			else if (htype == HTYPE_IntergasInComfortLAN2RF) {
				//All fine here
			}
			else if (htype == HTYPE_EnphaseAPI) {
				//all fine here!
			}
			else
				return;

			std::string mode1Str;
			std::string mode2Str;
			std::string mode3Str;
			std::string mode4Str;
			std::string mode5Str;
			std::string mode6Str;

			root["status"] = "OK";
			root["title"] = "UpdateHardware";

			if (htype == HTYPE_Domoticz)
			{
				if (password.size() != 32)
				{
					password = GenerateMD5Hash(password);
				}
			}

			if ((bIsSerial) && (!bEnabled) && (sport.empty()))
			{
				//just disable the device
				m_sql.safe_query(
					"UPDATE Hardware SET Enabled=%d WHERE (ID == '%q')",
					(bEnabled == true) ? 1 : 0,
					idx.c_str()
				);
			}
			else
			{
				if (htype == HTYPE_HTTPPOLLER) {
					m_sql.safe_query(
						"UPDATE Hardware SET Name='%q', Enabled=%d, Type=%d, Address='%q', Port=%d, SerialPort='%q', Username='%q', Password='%q', Extra='%q', DataTimeout=%d WHERE (ID == '%q')",
						name.c_str(),
						(senabled == "true") ? 1 : 0,
						htype,
						address.c_str(),
						port,
						sport.c_str(),
						username.c_str(),
						password.c_str(),
						extra.c_str(),
						iDataTimeout,
						idx.c_str()
					);
				}
				else if (htype == HTYPE_PythonPlugin) {
					mode1Str = request::findValue(&req, "Mode1");
					mode2Str = request::findValue(&req, "Mode2");
					mode3Str = request::findValue(&req, "Mode3");
					mode4Str = request::findValue(&req, "Mode4");
					mode5Str = request::findValue(&req, "Mode5");
					mode6Str = request::findValue(&req, "Mode6");
					sport = request::findValue(&req, "serialport");
					m_sql.safe_query(
						"UPDATE Hardware SET Name='%q', Enabled=%d, Type=%d, Address='%q', Port=%d, SerialPort='%q', Username='%q', Password='%q', Extra='%q', Mode1='%q', Mode2='%q', Mode3='%q', Mode4='%q', Mode5='%q', Mode6='%q', DataTimeout=%d WHERE (ID == '%q')",
						name.c_str(),
						(senabled == "true") ? 1 : 0,
						htype,
						address.c_str(),
						port,
						sport.c_str(),
						username.c_str(),
						password.c_str(),
						extra.c_str(),
						mode1Str.c_str(), mode2Str.c_str(), mode3Str.c_str(), mode4Str.c_str(), mode5Str.c_str(), mode6Str.c_str(),
						iDataTimeout,
						idx.c_str()
					);
				}
				else if (
					(htype == HTYPE_RFXtrx433) ||
					(htype == HTYPE_RFXtrx868)
					)
				{
					//No Extra field here, handled in CWebServer::SetRFXCOMMode
					m_sql.safe_query(
						"UPDATE Hardware SET Name='%q', Enabled=%d, Type=%d, Address='%q', Port=%d, SerialPort='%q', Username='%q', Password='%q', Mode1=%d, Mode2=%d, Mode3=%d, Mode4=%d, Mode5=%d, Mode6=%d, DataTimeout=%d WHERE (ID == '%q')",
						name.c_str(),
						(bEnabled == true) ? 1 : 0,
						htype,
						address.c_str(),
						port,
						sport.c_str(),
						username.c_str(),
						password.c_str(),
						mode1, mode2, mode3, mode4, mode5, mode6,
						iDataTimeout,
						idx.c_str()
					);
					std::vector<std::vector<std::string> > result;
					result = m_sql.safe_query("SELECT Extra FROM Hardware WHERE ID=%q", idx.c_str());
					if (!result.empty())
						extra = result[0][0];
				}
				else {
					m_sql.safe_query(
						"UPDATE Hardware SET Name='%q', Enabled=%d, Type=%d, Address='%q', Port=%d, SerialPort='%q', Username='%q', Password='%q', Extra='%q', Mode1=%d, Mode2=%d, Mode3=%d, Mode4=%d, Mode5=%d, Mode6=%d, DataTimeout=%d WHERE (ID == '%q')",
						name.c_str(),
						(bEnabled == true) ? 1 : 0,
						htype,
						address.c_str(),
						port,
						sport.c_str(),
						username.c_str(),
						password.c_str(),
						extra.c_str(),
						mode1, mode2, mode3, mode4, mode5, mode6,
						iDataTimeout,
						idx.c_str()
					);
				}
			}

			//re-add the device in our system
			int ID = atoi(idx.c_str());
			m_mainworker.AddHardwareFromParams(ID, name, bEnabled, htype, address, port, sport, username, password, extra, mode1, mode2, mode3, mode4, mode5, mode6, iDataTimeout, true);
		}