		void CWebServer::PostSettings(WebEmSession & session, const request& req, std::string & redirect_uri)
		{
			redirect_uri = "/index.html";

			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string Latitude = request::findValue(&req, "Latitude");
			std::string Longitude = request::findValue(&req, "Longitude");
			if ((Latitude != "") && (Longitude != ""))
			{
				std::string LatLong = Latitude + ";" + Longitude;
				m_sql.UpdatePreferencesVar("Location", LatLong.c_str());
				m_mainworker.GetSunSettings();
			}
			m_notifications.ConfigFromGetvars(req, true);
			std::string DashboardType = request::findValue(&req, "DashboardType");
			m_sql.UpdatePreferencesVar("DashboardType", atoi(DashboardType.c_str()));
			std::string MobileType = request::findValue(&req, "MobileType");
			m_sql.UpdatePreferencesVar("MobileType", atoi(MobileType.c_str()));

			int nUnit = atoi(request::findValue(&req, "WindUnit").c_str());
			m_sql.UpdatePreferencesVar("WindUnit", nUnit);
			m_sql.m_windunit = (_eWindUnit)nUnit;

			nUnit = atoi(request::findValue(&req, "TempUnit").c_str());
			m_sql.UpdatePreferencesVar("TempUnit", nUnit);
			m_sql.m_tempunit = (_eTempUnit)nUnit;

			nUnit = atoi(request::findValue(&req, "WeightUnit").c_str());
			m_sql.UpdatePreferencesVar("WeightUnit", nUnit);
			m_sql.m_weightunit = (_eWeightUnit)nUnit;


			m_sql.SetUnitsAndScale();

			std::string AuthenticationMethod = request::findValue(&req, "AuthenticationMethod");
			_eAuthenticationMethod amethod = (_eAuthenticationMethod)atoi(AuthenticationMethod.c_str());
			m_sql.UpdatePreferencesVar("AuthenticationMethod", static_cast<int>(amethod));
			m_pWebEm->SetAuthenticationMethod(amethod);

			std::string ReleaseChannel = request::findValue(&req, "ReleaseChannel");
			m_sql.UpdatePreferencesVar("ReleaseChannel", atoi(ReleaseChannel.c_str()));

			std::string LightHistoryDays = request::findValue(&req, "LightHistoryDays");
			m_sql.UpdatePreferencesVar("LightHistoryDays", atoi(LightHistoryDays.c_str()));

			std::string s5MinuteHistoryDays = request::findValue(&req, "ShortLogDays");
			m_sql.UpdatePreferencesVar("5MinuteHistoryDays", atoi(s5MinuteHistoryDays.c_str()));

			int iShortLogInterval = atoi(request::findValue(&req, "ShortLogInterval").c_str());
			if (iShortLogInterval < 1)
				iShortLogInterval = 5;
			m_sql.UpdatePreferencesVar("ShortLogInterval", iShortLogInterval);
			m_sql.m_ShortLogInterval = iShortLogInterval;

			std::string sElectricVoltage = request::findValue(&req, "ElectricVoltage");
			m_sql.UpdatePreferencesVar("ElectricVoltage", atoi(sElectricVoltage.c_str()));

			std::string sCM113DisplayType = request::findValue(&req, "CM113DisplayType");
			m_sql.UpdatePreferencesVar("CM113DisplayType", atoi(sCM113DisplayType.c_str()));

			std::string WebUserName = base64_encode(CURLEncode::URLDecode(request::findValue(&req, "WebUserName")));
			std::string WebPassword = CURLEncode::URLDecode(request::findValue(&req, "WebPassword"));

			//Get old username/password
			std::string sOldWebLogin;
			std::string sOldWebPassword;
			m_sql.GetPreferencesVar("WebUserName", sOldWebLogin);
			m_sql.GetPreferencesVar("WebPassword", sOldWebPassword);

			bool bHaveAdminUserPasswordChange = false;

			if ((WebUserName == sOldWebLogin) && (WebPassword.empty()))
			{
				//All is OK, no changes
			}
			else if (WebUserName.empty() || WebPassword.empty())
			{
				//If no Admin User/Password is specified, we clear them
				if ((!sOldWebLogin.empty()) || (!sOldWebPassword.empty()))
					bHaveAdminUserPasswordChange = true;
				WebUserName = "";
				WebPassword = "";
			}
			else {
				if ((WebUserName != sOldWebLogin) || (WebPassword != sOldWebPassword))
				{
					bHaveAdminUserPasswordChange = true;
				}
			}

			// Invalid sessions of WebUser when the username or password has been changed
			if (bHaveAdminUserPasswordChange)
			{
				RemoveUsersSessions(sOldWebLogin, session);
				m_sql.UpdatePreferencesVar("WebUserName", WebUserName.c_str());
				m_sql.UpdatePreferencesVar("WebPassword", WebPassword.c_str());
			}

			std::string WebLocalNetworks = CURLEncode::URLDecode(request::findValue(&req, "WebLocalNetworks"));
			std::string WebRemoteProxyIPs = CURLEncode::URLDecode(request::findValue(&req, "WebRemoteProxyIPs"));
			m_sql.UpdatePreferencesVar("WebLocalNetworks", WebLocalNetworks.c_str());
			m_sql.UpdatePreferencesVar("WebRemoteProxyIPs", WebRemoteProxyIPs.c_str());

			LoadUsers();
			m_pWebEm->ClearLocalNetworks();
			std::vector<std::string> strarray;
			StringSplit(WebLocalNetworks, ";", strarray);
			for (const auto & itt : strarray)
				m_pWebEm->AddLocalNetworks(itt);
			//add local hostname
			m_pWebEm->AddLocalNetworks("");

			m_pWebEm->ClearRemoteProxyIPs();
			strarray.clear();
			StringSplit(WebRemoteProxyIPs, ";", strarray);
			for (const auto & itt : strarray)
				m_pWebEm->AddRemoteProxyIPs(itt);

			if (session.username.empty())
			{
				//Local network could be changed so lets for a check here
				session.rights = -1;
			}

			std::string SecPassword = request::findValue(&req, "SecPassword");
			SecPassword = CURLEncode::URLDecode(SecPassword);
			if (SecPassword.size() != 32)
			{
				SecPassword = GenerateMD5Hash(SecPassword);
			}
			m_sql.UpdatePreferencesVar("SecPassword", SecPassword.c_str());

			std::string ProtectionPassword = request::findValue(&req, "ProtectionPassword");
			ProtectionPassword = CURLEncode::URLDecode(ProtectionPassword);
			if (ProtectionPassword.size() != 32)
			{
				ProtectionPassword = GenerateMD5Hash(ProtectionPassword);
			}
			m_sql.UpdatePreferencesVar("ProtectionPassword", ProtectionPassword.c_str());

			int EnergyDivider = atoi(request::findValue(&req, "EnergyDivider").c_str());
			int GasDivider = atoi(request::findValue(&req, "GasDivider").c_str());
			int WaterDivider = atoi(request::findValue(&req, "WaterDivider").c_str());
			if (EnergyDivider < 1)
				EnergyDivider = 1000;
			if (GasDivider < 1)
				GasDivider = 100;
			if (WaterDivider < 1)
				WaterDivider = 100;
			m_sql.UpdatePreferencesVar("MeterDividerEnergy", EnergyDivider);
			m_sql.UpdatePreferencesVar("MeterDividerGas", GasDivider);
			m_sql.UpdatePreferencesVar("MeterDividerWater", WaterDivider);

			std::string scheckforupdates = request::findValue(&req, "checkforupdates");
			m_sql.UpdatePreferencesVar("UseAutoUpdate", (scheckforupdates == "on" ? 1 : 0));

			std::string senableautobackup = request::findValue(&req, "enableautobackup");
			m_sql.UpdatePreferencesVar("UseAutoBackup", (senableautobackup == "on" ? 1 : 0));

			float CostEnergy = static_cast<float>(atof(request::findValue(&req, "CostEnergy").c_str()));
			float CostEnergyT2 = static_cast<float>(atof(request::findValue(&req, "CostEnergyT2").c_str()));
			float CostEnergyR1 = static_cast<float>(atof(request::findValue(&req, "CostEnergyR1").c_str()));
			float CostEnergyR2 = static_cast<float>(atof(request::findValue(&req, "CostEnergyR2").c_str()));
			float CostGas = static_cast<float>(atof(request::findValue(&req, "CostGas").c_str()));
			float CostWater = static_cast<float>(atof(request::findValue(&req, "CostWater").c_str()));
			m_sql.UpdatePreferencesVar("CostEnergy", int(CostEnergy*10000.0f));
			m_sql.UpdatePreferencesVar("CostEnergyT2", int(CostEnergyT2*10000.0f));
			m_sql.UpdatePreferencesVar("CostEnergyR1", int(CostEnergyR1*10000.0f));
			m_sql.UpdatePreferencesVar("CostEnergyR2", int(CostEnergyR2*10000.0f));
			m_sql.UpdatePreferencesVar("CostGas", int(CostGas*10000.0f));
			m_sql.UpdatePreferencesVar("CostWater", int(CostWater*10000.0f));

			int rnOldvalue = 0;
			int rnvalue = 0;

			m_sql.GetPreferencesVar("ActiveTimerPlan", rnOldvalue);
			rnvalue = atoi(request::findValue(&req, "ActiveTimerPlan").c_str());
			if (rnOldvalue != rnvalue)
			{
				m_sql.UpdatePreferencesVar("ActiveTimerPlan", rnvalue);
				m_sql.m_ActiveTimerPlan = rnvalue;
				m_mainworker.m_scheduler.ReloadSchedules();
			}
			m_sql.UpdatePreferencesVar("DoorbellCommand", atoi(request::findValue(&req, "DoorbellCommand").c_str()));
			m_sql.UpdatePreferencesVar("SmartMeterType", atoi(request::findValue(&req, "SmartMeterType").c_str()));

			std::string EnableTabFloorplans = request::findValue(&req, "EnableTabFloorplans");
			m_sql.UpdatePreferencesVar("EnableTabFloorplans", (EnableTabFloorplans == "on" ? 1 : 0));
			std::string EnableTabLights = request::findValue(&req, "EnableTabLights");
			m_sql.UpdatePreferencesVar("EnableTabLights", (EnableTabLights == "on" ? 1 : 0));
			std::string EnableTabTemp = request::findValue(&req, "EnableTabTemp");
			m_sql.UpdatePreferencesVar("EnableTabTemp", (EnableTabTemp == "on" ? 1 : 0));
			std::string EnableTabWeather = request::findValue(&req, "EnableTabWeather");
			m_sql.UpdatePreferencesVar("EnableTabWeather", (EnableTabWeather == "on" ? 1 : 0));
			std::string EnableTabUtility = request::findValue(&req, "EnableTabUtility");
			m_sql.UpdatePreferencesVar("EnableTabUtility", (EnableTabUtility == "on" ? 1 : 0));
			std::string EnableTabScenes = request::findValue(&req, "EnableTabScenes");
			m_sql.UpdatePreferencesVar("EnableTabScenes", (EnableTabScenes == "on" ? 1 : 0));
			std::string EnableTabCustom = request::findValue(&req, "EnableTabCustom");
			m_sql.UpdatePreferencesVar("EnableTabCustom", (EnableTabCustom == "on" ? 1 : 0));

			m_sql.GetPreferencesVar("NotificationSensorInterval", rnOldvalue);
			rnvalue = atoi(request::findValue(&req, "NotificationSensorInterval").c_str());
			if (rnOldvalue != rnvalue)
			{
				m_sql.UpdatePreferencesVar("NotificationSensorInterval", rnvalue);
				m_notifications.ReloadNotifications();
			}
			m_sql.GetPreferencesVar("NotificationSwitchInterval", rnOldvalue);
			rnvalue = atoi(request::findValue(&req, "NotificationSwitchInterval").c_str());
			if (rnOldvalue != rnvalue)
			{
				m_sql.UpdatePreferencesVar("NotificationSwitchInterval", rnvalue);
				m_notifications.ReloadNotifications();
			}
			std::string RaspCamParams = request::findValue(&req, "RaspCamParams");
			if (RaspCamParams != "")
			{
				if (IsArgumentSecure(RaspCamParams))
					m_sql.UpdatePreferencesVar("RaspCamParams", RaspCamParams.c_str());
			}

			std::string UVCParams = request::findValue(&req, "UVCParams");
			if (UVCParams != "")
			{
				if (IsArgumentSecure(UVCParams))
					m_sql.UpdatePreferencesVar("UVCParams", UVCParams.c_str());
			}

			std::string EnableNewHardware = request::findValue(&req, "AcceptNewHardware");
			int iEnableNewHardware = (EnableNewHardware == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("AcceptNewHardware", iEnableNewHardware);
			m_sql.m_bAcceptNewHardware = (iEnableNewHardware == 1);

			std::string HideDisabledHardwareSensors = request::findValue(&req, "HideDisabledHardwareSensors");
			int iHideDisabledHardwareSensors = (HideDisabledHardwareSensors == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("HideDisabledHardwareSensors", iHideDisabledHardwareSensors);

			std::string ShowUpdateEffect = request::findValue(&req, "ShowUpdateEffect");
			int iShowUpdateEffect = (ShowUpdateEffect == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("ShowUpdateEffect", iShowUpdateEffect);

			std::string SendErrorsAsNotification = request::findValue(&req, "SendErrorsAsNotification");
			int iSendErrorsAsNotification = (SendErrorsAsNotification == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("SendErrorsAsNotification", iSendErrorsAsNotification);
			_log.ForwardErrorsToNotificationSystem(iSendErrorsAsNotification != 0);

			std::string DegreeDaysBaseTemperature = request::findValue(&req, "DegreeDaysBaseTemperature");
			m_sql.UpdatePreferencesVar("DegreeDaysBaseTemperature", DegreeDaysBaseTemperature);

			rnOldvalue = 0;
			m_sql.GetPreferencesVar("EnableEventScriptSystem", rnOldvalue);
			std::string EnableEventScriptSystem = request::findValue(&req, "EnableEventScriptSystem");
			int iEnableEventScriptSystem = (EnableEventScriptSystem == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("EnableEventScriptSystem", iEnableEventScriptSystem);
			m_sql.m_bEnableEventSystem = (iEnableEventScriptSystem == 1);
			if (iEnableEventScriptSystem != rnOldvalue)
			{
				m_mainworker.m_eventsystem.SetEnabled(m_sql.m_bEnableEventSystem);
				m_mainworker.m_eventsystem.StartEventSystem();
			}

			rnOldvalue = 0;
			m_sql.GetPreferencesVar("DisableDzVentsSystem", rnOldvalue);
			std::string DisableDzVentsSystem = request::findValue(&req, "DisableDzVentsSystem");
			int iDisableDzVentsSystem = (DisableDzVentsSystem == "on" ? 0 : 1);
			m_sql.UpdatePreferencesVar("DisableDzVentsSystem", iDisableDzVentsSystem);
			m_sql.m_bDisableDzVentsSystem = (iDisableDzVentsSystem == 1);
			if (m_sql.m_bEnableEventSystem && !iDisableDzVentsSystem && iDisableDzVentsSystem != rnOldvalue)
			{
				m_mainworker.m_eventsystem.LoadEvents();
				m_mainworker.m_eventsystem.GetCurrentStates();
			}
			m_sql.UpdatePreferencesVar("DzVentsLogLevel", atoi(request::findValue(&req, "DzVentsLogLevel").c_str()));

			std::string LogEventScriptTrigger = request::findValue(&req, "LogEventScriptTrigger");
			m_sql.m_bLogEventScriptTrigger = (LogEventScriptTrigger == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("LogEventScriptTrigger", m_sql.m_bLogEventScriptTrigger);

			std::string EnableWidgetOrdering = request::findValue(&req, "AllowWidgetOrdering");
			int iEnableAllowWidgetOrdering = (EnableWidgetOrdering == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("AllowWidgetOrdering", iEnableAllowWidgetOrdering);
			m_sql.m_bAllowWidgetOrdering = (iEnableAllowWidgetOrdering == 1);

			rnOldvalue = 0;
			m_sql.GetPreferencesVar("RemoteSharedPort", rnOldvalue);

			m_sql.UpdatePreferencesVar("RemoteSharedPort", atoi(request::findValue(&req, "RemoteSharedPort").c_str()));

			rnvalue = 0;
			m_sql.GetPreferencesVar("RemoteSharedPort", rnvalue);

			if (rnvalue != rnOldvalue)
			{
				m_mainworker.m_sharedserver.StopServer();
				if (rnvalue != 0)
				{
					char szPort[100];
					sprintf(szPort, "%d", rnvalue);
					m_mainworker.m_sharedserver.StartServer("::", szPort);
					m_mainworker.LoadSharedUsers();
				}
			}

			m_sql.UpdatePreferencesVar("Language", request::findValue(&req, "Language").c_str());
			std::string SelectedTheme = request::findValue(&req, "Themes");
			m_sql.UpdatePreferencesVar("WebTheme", SelectedTheme.c_str());
			m_pWebEm->SetWebTheme(SelectedTheme);
			std::string Title = request::findValue(&req, "Title").c_str();
			m_sql.UpdatePreferencesVar("Title", (Title.empty()) ? "Domoticz" : Title);

			m_sql.GetPreferencesVar("RandomTimerFrame", rnOldvalue);
			rnvalue = atoi(request::findValue(&req, "RandomSpread").c_str());
			if (rnOldvalue != rnvalue)
			{
				m_sql.UpdatePreferencesVar("RandomTimerFrame", rnvalue);
				m_mainworker.m_scheduler.ReloadSchedules();
			}

			m_sql.UpdatePreferencesVar("SecOnDelay", atoi(request::findValue(&req, "SecOnDelay").c_str()));

			int sensortimeout = atoi(request::findValue(&req, "SensorTimeout").c_str());
			if (sensortimeout < 10)
				sensortimeout = 10;
			m_sql.UpdatePreferencesVar("SensorTimeout", sensortimeout);

			int batterylowlevel = atoi(request::findValue(&req, "BatterLowLevel").c_str());
			if (batterylowlevel > 100)
				batterylowlevel = 100;
			m_sql.GetPreferencesVar("BatteryLowNotification", rnOldvalue);
			m_sql.UpdatePreferencesVar("BatteryLowNotification", batterylowlevel);
			if ((rnOldvalue != batterylowlevel) && (batterylowlevel != 0))
				m_sql.CheckBatteryLow();

			int nValue = 0;
			nValue = atoi(request::findValue(&req, "FloorplanPopupDelay").c_str());
			m_sql.UpdatePreferencesVar("FloorplanPopupDelay", nValue);
			std::string FloorplanFullscreenMode = request::findValue(&req, "FloorplanFullscreenMode");
			m_sql.UpdatePreferencesVar("FloorplanFullscreenMode", (FloorplanFullscreenMode == "on" ? 1 : 0));
			std::string FloorplanAnimateZoom = request::findValue(&req, "FloorplanAnimateZoom");
			m_sql.UpdatePreferencesVar("FloorplanAnimateZoom", (FloorplanAnimateZoom == "on" ? 1 : 0));
			std::string FloorplanShowSensorValues = request::findValue(&req, "FloorplanShowSensorValues");
			m_sql.UpdatePreferencesVar("FloorplanShowSensorValues", (FloorplanShowSensorValues == "on" ? 1 : 0));
			std::string FloorplanShowSwitchValues = request::findValue(&req, "FloorplanShowSwitchValues");
			m_sql.UpdatePreferencesVar("FloorplanShowSwitchValues", (FloorplanShowSwitchValues == "on" ? 1 : 0));
			std::string FloorplanShowSceneNames = request::findValue(&req, "FloorplanShowSceneNames");
			m_sql.UpdatePreferencesVar("FloorplanShowSceneNames", (FloorplanShowSceneNames == "on" ? 1 : 0));
			m_sql.UpdatePreferencesVar("FloorplanRoomColour", CURLEncode::URLDecode(request::findValue(&req, "FloorplanRoomColour").c_str()).c_str());
			m_sql.UpdatePreferencesVar("FloorplanActiveOpacity", atoi(request::findValue(&req, "FloorplanActiveOpacity").c_str()));
			m_sql.UpdatePreferencesVar("FloorplanInactiveOpacity", atoi(request::findValue(&req, "FloorplanInactiveOpacity").c_str()));

#ifndef NOCLOUD
			std::string md_userid, md_password, pf_userid, pf_password;
			int md_subsystems, pf_subsystems;
			m_sql.GetPreferencesVar("MyDomoticzUserId", pf_userid);
			m_sql.GetPreferencesVar("MyDomoticzPassword", pf_password);
			m_sql.GetPreferencesVar("MyDomoticzSubsystems", pf_subsystems);
			md_userid = CURLEncode::URLDecode(request::findValue(&req, "MyDomoticzUserId"));
			md_password = CURLEncode::URLDecode(request::findValue(&req, "MyDomoticzPassword"));
			md_subsystems = (request::findValue(&req, "SubsystemHttp").empty() ? 0 : 1) + (request::findValue(&req, "SubsystemShared").empty() ? 0 : 2) + (request::findValue(&req, "SubsystemApps").empty() ? 0 : 4);
			if (md_userid != pf_userid || md_password != pf_password || md_subsystems != pf_subsystems) {
				m_sql.UpdatePreferencesVar("MyDomoticzUserId", md_userid);
				if (md_password != pf_password) {
					md_password = base64_encode(md_password);
					m_sql.UpdatePreferencesVar("MyDomoticzPassword", md_password);
				}
				m_sql.UpdatePreferencesVar("MyDomoticzSubsystems", md_subsystems);
				m_webservers.RestartProxy();
			}
#endif

			m_sql.UpdatePreferencesVar("OneWireSensorPollPeriod", atoi(request::findValue(&req, "OneWireSensorPollPeriod").c_str()));
			m_sql.UpdatePreferencesVar("OneWireSwitchPollPeriod", atoi(request::findValue(&req, "OneWireSwitchPollPeriod").c_str()));

			std::string IFTTTEnabled = request::findValue(&req, "IFTTTEnabled");
			int iIFTTTEnabled = (IFTTTEnabled == "on" ? 1 : 0);
			m_sql.UpdatePreferencesVar("IFTTTEnabled", iIFTTTEnabled);
			std::string szKey = request::findValue(&req, "IFTTTAPI");
			m_sql.UpdatePreferencesVar("IFTTTAPI", base64_encode(szKey));

			m_notifications.LoadConfig();
#ifdef ENABLE_PYTHON
			//Signal plugins to update Settings dictionary
			PluginLoadConfig();
#endif
		}