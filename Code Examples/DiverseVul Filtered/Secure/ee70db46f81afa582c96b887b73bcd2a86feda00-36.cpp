		void CWebServer::Cmd_AddHardware(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

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
			_eHardwareTypes htype = (_eHardwareTypes)atoi(shtype.c_str());

			int iDataTimeout = atoi(sdatatimeout.c_str());
			int mode1 = 0;
			int mode2 = 0;
			int mode3 = 0;
			int mode4 = 0;
			int mode5 = 0;
			int mode6 = 0;
			int port = atoi(sport.c_str());
			std::string mode1Str = request::findValue(&req, "Mode1");
			if (!mode1Str.empty()) {
				mode1 = atoi(mode1Str.c_str());
			}
			std::string mode2Str = request::findValue(&req, "Mode2");
			if (!mode2Str.empty()) {
				mode2 = atoi(mode2Str.c_str());
			}
			std::string mode3Str = request::findValue(&req, "Mode3");
			if (!mode3Str.empty()) {
				mode3 = atoi(mode3Str.c_str());
			}
			std::string mode4Str = request::findValue(&req, "Mode4");
			if (!mode4Str.empty()) {
				mode4 = atoi(mode4Str.c_str());
			}
			std::string mode5Str = request::findValue(&req, "Mode5");
			if (!mode5Str.empty()) {
				mode5 = atoi(mode5Str.c_str());
			}
			std::string mode6Str = request::findValue(&req, "Mode6");
			if (!mode6Str.empty()) {
				mode6 = atoi(mode6Str.c_str());
			}

			if (IsSerialDevice(htype))
			{
				//USB/System
				if (sport.empty())
					return; //need to have a serial port

				if (htype == HTYPE_TeleinfoMeter) {
					// Teleinfo always has decimals. Chances to have a P1 and a Teleinfo device on the same
					// Domoticz instance are very low as both are national standards (NL and FR)
					m_sql.UpdatePreferencesVar("SmartMeterType", 0);
				}
			}
			else if (IsNetworkDevice(htype))
			{
				//Lan
				if (address.empty() || port == 0)
					return;

				if (htype == HTYPE_MySensorsMQTT || htype == HTYPE_MQTT) {
					std::string modeqStr = request::findValue(&req, "mode1");
					if (!modeqStr.empty()) {
						mode1 = atoi(modeqStr.c_str());
					}
				}

				if (htype == HTYPE_ECODEVICES) {
					// EcoDevices always have decimals. Chances to have a P1 and a EcoDevice/Teleinfo device on the same
					// Domoticz instance are very low as both are national standards (NL and FR)
					m_sql.UpdatePreferencesVar("SmartMeterType", 0);
				}
			}
			else if (htype == HTYPE_DomoticzInternal) {
				// DomoticzInternal cannot be added manually
				return;
			}
			else if (htype == HTYPE_Domoticz) {
				//Remote Domoticz
				if (address.empty() || port == 0)
					return;
			}
			else if (htype == HTYPE_TE923) {
				//all fine here!
			}
			else if (htype == HTYPE_VOLCRAFTCO20) {
				//all fine here!
			}
			else if (htype == HTYPE_System) {
				//There should be only one
				std::vector<std::vector<std::string> > result;
				result = m_sql.safe_query("SELECT ID FROM Hardware WHERE (Type==%d)", HTYPE_System);
				if (!result.empty())
					return;
			}
			else if (htype == HTYPE_1WIRE) {
				//all fine here!
			}
			else if (htype == HTYPE_Rtl433) {
				//all fine here!
			}
			else if (htype == HTYPE_Pinger) {
				//all fine here!
			}
			else if (htype == HTYPE_Kodi) {
				//all fine here!
			}
			else if (htype == HTYPE_PanasonicTV) {
				// all fine here!
			}
			else if (htype == HTYPE_LogitechMediaServer) {
				//all fine here!
			}
			else if (htype == HTYPE_RaspberryBMP085) {
				//all fine here!
			}
			else if (htype == HTYPE_RaspberryHTU21D) {
				//all fine here!
			}
			else if (htype == HTYPE_RaspberryTSL2561) {
				//all fine here!
			}
			else if (htype == HTYPE_RaspberryBME280) {
				//all fine here!
			}
			else if (htype == HTYPE_RaspberryMCP23017) {
				//all fine here!
			}
			else if (htype == HTYPE_Dummy) {
				//all fine here!
			}
			else if (htype == HTYPE_Tellstick) {
				//all fine here!
			}
			else if (htype == HTYPE_EVOHOME_SCRIPT || htype == HTYPE_EVOHOME_SERIAL || htype == HTYPE_EVOHOME_WEB || htype == HTYPE_EVOHOME_TCP) {
				//all fine here!
			}
			else if (htype == HTYPE_PiFace) {
				//all fine here!
			}
			else if (htype == HTYPE_HTTPPOLLER) {
				//all fine here!
			}
			else if (htype == HTYPE_BleBox) {
				//all fine here!
			}
			else if (htype == HTYPE_HEOS) {
				//all fine here!
			}
			else if (htype == HTYPE_Yeelight) {
				//all fine here!
			}
			else if (htype == HTYPE_XiaomiGateway) {
				//all fine here!
			}
			else if (htype == HTYPE_Arilux) {
				//all fine here!
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
			else if (htype == HTYPE_SBFSpot) {
				if (username.empty())
					return;
			}
			else if (htype == HTYPE_HARMONY_HUB) {
				if (
					(address.empty() || port == 0)
					)
					return;
			}
			else if (htype == HTYPE_Philips_Hue) {
				if (
					(username.empty()) ||
					(address.empty() || port == 0)
					)
					return;
				if (port == 0)
					port = 80;
			}
			else if (htype == HTYPE_WINDDELEN) {
				std::string mill_id = request::findValue(&req, "Mode1");
				if (
					(mill_id.empty()) ||
					(sport.empty())
					)

					return;
				mode1 = atoi(mill_id.c_str());
			}
			else if (htype == HTYPE_Honeywell) {
				//all fine here!
			}
			else if (htype == HTYPE_RaspberryGPIO) {
				//all fine here!
			}
			else if (htype == HTYPE_SysfsGpio) {
				//all fine here!
			}
			else if (htype == HTYPE_OpenWebNetTCP) {
				//All fine here
			}
			else if (htype == HTYPE_Daikin) {
				//All fine here
			}
			else if (htype == HTYPE_GoodweAPI) {
				if (username.empty())
					return;
			}
			else if (htype == HTYPE_PythonPlugin) {
				//All fine here
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
				//All fine here
			}
			else if (htype == HTYPE_EcoCompteur) {
				//all fine here!
			}
			else
				return;

			root["status"] = "OK";
			root["title"] = "AddHardware";

			std::vector<std::vector<std::string> > result;

			if (htype == HTYPE_Domoticz)
			{
				if (password.size() != 32)
				{
					password = GenerateMD5Hash(password);
				}
			}
			else if ((htype == HTYPE_S0SmartMeterUSB) || (htype == HTYPE_S0SmartMeterTCP))
			{
				extra = "0;1000;0;1000;0;1000;0;1000;0;1000";
			}
			else if (htype == HTYPE_Pinger)
			{
				mode1 = 30;
				mode2 = 1000;
			}
			else if (htype == HTYPE_Kodi)
			{
				mode1 = 30;
				mode2 = 1000;
			}
			else if (htype == HTYPE_PanasonicTV)
			{
				mode1 = 30;
				mode2 = 1000;
			}
			else if (htype == HTYPE_LogitechMediaServer)
			{
				mode1 = 30;
				mode2 = 1000;
			}
			else if (htype == HTYPE_HEOS)
			{
				mode1 = 30;
				mode2 = 1000;
			}
			else if (htype == HTYPE_Tellstick)
			{
				mode1 = 4;
				mode2 = 500;
			}

			if (htype == HTYPE_HTTPPOLLER) {
				m_sql.safe_query(
					"INSERT INTO Hardware (Name, Enabled, Type, Address, Port, SerialPort, Username, Password, Extra, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6, DataTimeout) VALUES ('%q',%d, %d,'%q',%d,'%q','%q','%q','%q','%q','%q', '%q', '%q', '%q', '%q', %d)",
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
					iDataTimeout
				);
			}
			else if (htype == HTYPE_PythonPlugin) {
				sport = request::findValue(&req, "serialport");
				m_sql.safe_query(
					"INSERT INTO Hardware (Name, Enabled, Type, Address, Port, SerialPort, Username, Password, Extra, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6, DataTimeout) VALUES ('%q',%d, %d,'%q',%d,'%q','%q','%q','%q','%q','%q', '%q', '%q', '%q', '%q', %d)",
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
					iDataTimeout
				);
			}
			else if (
				(htype == HTYPE_RFXtrx433)||
				(htype == HTYPE_RFXtrx868)
				)
			{
				//No Extra field here, handled in CWebServer::SetRFXCOMMode
				m_sql.safe_query(
					"INSERT INTO Hardware (Name, Enabled, Type, Address, Port, SerialPort, Username, Password, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6, DataTimeout) VALUES ('%q',%d, %d,'%q',%d,'%q','%q','%q',%d,%d,%d,%d,%d,%d,%d)",
					name.c_str(),
					(senabled == "true") ? 1 : 0,
					htype,
					address.c_str(),
					port,
					sport.c_str(),
					username.c_str(),
					password.c_str(),
					mode1, mode2, mode3, mode4, mode5, mode6,
					iDataTimeout
				);
				extra = "0";
			}
			else {
				m_sql.safe_query(
					"INSERT INTO Hardware (Name, Enabled, Type, Address, Port, SerialPort, Username, Password, Extra, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6, DataTimeout) VALUES ('%q',%d, %d,'%q',%d,'%q','%q','%q','%q',%d,%d,%d,%d,%d,%d,%d)",
					name.c_str(),
					(senabled == "true") ? 1 : 0,
					htype,
					address.c_str(),
					port,
					sport.c_str(),
					username.c_str(),
					password.c_str(),
					extra.c_str(),
					mode1, mode2, mode3, mode4, mode5, mode6,
					iDataTimeout
				);
			}

			//add the device for real in our system
			result = m_sql.safe_query("SELECT MAX(ID) FROM Hardware");
			if (!result.empty())
			{
				std::vector<std::string> sd = result[0];
				int ID = atoi(sd[0].c_str());

				root["idx"] = sd[0].c_str(); // OTO output the created ID for easier management on the caller side (if automated)

				m_mainworker.AddHardwareFromParams(ID, name, (senabled == "true") ? true : false, htype, address, port, sport, username, password, extra, mode1, mode2, mode3, mode4, mode5, mode6, iDataTimeout, true);
			}
		}