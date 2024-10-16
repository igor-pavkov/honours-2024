		bool CWebServer::StartServer(server_settings & settings, const std::string & serverpath, const bool bIgnoreUsernamePassword)
		{
			m_server_alias = (settings.is_secure() == true) ? "SSL" : "HTTP";

			if (!settings.is_enabled())
				return true;

			ReloadCustomSwitchIcons();

			int tries = 0;
			bool exception = false;

			//_log.Log(LOG_STATUS, "CWebServer::StartServer() : settings : %s", settings.to_string().c_str());
			do {
				try {
					exception = false;
					m_pWebEm = new http::server::cWebem(settings, serverpath.c_str());
				}
				catch (std::exception& e) {
					exception = true;
					switch (tries) {
					case 0:
						settings.listening_address = "::";
						break;
					case 1:
						settings.listening_address = "0.0.0.0";
						break;
					case 2:
						_log.Log(LOG_ERROR, "WebServer(%s) startup failed on address %s with port: %s: %s", m_server_alias.c_str(), settings.listening_address.c_str(), settings.listening_port.c_str(), e.what());
						if (atoi(settings.listening_port.c_str()) < 1024)
							_log.Log(LOG_ERROR, "WebServer(%s) check privileges for opening ports below 1024", m_server_alias.c_str());
						else
							_log.Log(LOG_ERROR, "WebServer(%s) check if no other application is using port: %s", m_server_alias.c_str(), settings.listening_port.c_str());
						return false;
					}
					tries++;
				}
			} while (exception);

			_log.Log(LOG_STATUS, "WebServer(%s) started on address: %s with port %s", m_server_alias.c_str(), settings.listening_address.c_str(), settings.listening_port.c_str());

			m_pWebEm->SetDigistRealm("Domoticz.com");
			m_pWebEm->SetSessionStore(this);

			if (!bIgnoreUsernamePassword)
			{
				LoadUsers();
				std::string WebLocalNetworks;
				int nValue;
				if (m_sql.GetPreferencesVar("WebLocalNetworks", nValue, WebLocalNetworks))
				{
					std::vector<std::string> strarray;
					StringSplit(WebLocalNetworks, ";", strarray);
					for (const auto & itt : strarray)
						m_pWebEm->AddLocalNetworks(itt);
					//add local hostname
					m_pWebEm->AddLocalNetworks("");
				}
			}

			std::string WebRemoteProxyIPs;
			int nValue;
			if (m_sql.GetPreferencesVar("WebRemoteProxyIPs", nValue, WebRemoteProxyIPs))
			{
				std::vector<std::string> strarray;
				StringSplit(WebRemoteProxyIPs, ";", strarray);
				for (const auto & itt : strarray)
					m_pWebEm->AddRemoteProxyIPs(itt);
			}

			//register callbacks
			m_pWebEm->RegisterIncludeCode("switchtypes", boost::bind(&CWebServer::DisplaySwitchTypesCombo, this, _1));
			m_pWebEm->RegisterIncludeCode("metertypes", boost::bind(&CWebServer::DisplayMeterTypesCombo, this, _1));
			m_pWebEm->RegisterIncludeCode("timertypes", boost::bind(&CWebServer::DisplayTimerTypesCombo, this, _1));
			m_pWebEm->RegisterIncludeCode("combolanguage", boost::bind(&CWebServer::DisplayLanguageCombo, this, _1));

			m_pWebEm->RegisterPageCode("/json.htm", boost::bind(&CWebServer::GetJSonPage, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/uploadcustomicon", boost::bind(&CWebServer::Post_UploadCustomIcon, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/html5.appcache", boost::bind(&CWebServer::GetAppCache, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/camsnapshot.jpg", boost::bind(&CWebServer::GetCameraSnapshot, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/backupdatabase.php", boost::bind(&CWebServer::GetDatabaseBackup, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/raspberry.cgi", boost::bind(&CWebServer::GetInternalCameraSnapshot, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/uvccapture.cgi", boost::bind(&CWebServer::GetInternalCameraSnapshot, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/images/floorplans/plan", boost::bind(&CWebServer::GetFloorplanImage, this, _1, _2, _3));

			m_pWebEm->RegisterActionCode("storesettings", boost::bind(&CWebServer::PostSettings, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("setrfxcommode", boost::bind(&CWebServer::SetRFXCOMMode, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("rfxupgradefirmware", boost::bind(&CWebServer::RFXComUpgradeFirmware, this, _1, _2, _3));
			RegisterCommandCode("rfxfirmwaregetpercentage", boost::bind(&CWebServer::Cmd_RFXComGetFirmwarePercentage, this, _1, _2, _3), true);
			m_pWebEm->RegisterActionCode("setrego6xxtype", boost::bind(&CWebServer::SetRego6XXType, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("sets0metertype", boost::bind(&CWebServer::SetS0MeterType, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("setlimitlesstype", boost::bind(&CWebServer::SetLimitlessType, this, _1, _2, _3));

			m_pWebEm->RegisterActionCode("uploadfloorplanimage", boost::bind(&CWebServer::UploadFloorplanImage, this, _1, _2, _3));


			m_pWebEm->RegisterActionCode("setopenthermsettings", boost::bind(&CWebServer::SetOpenThermSettings, this, _1, _2, _3));
			RegisterCommandCode("sendopenthermcommand", boost::bind(&CWebServer::Cmd_SendOpenThermCommand, this, _1, _2, _3), true);

			m_pWebEm->RegisterActionCode("reloadpiface", boost::bind(&CWebServer::ReloadPiFace, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("setcurrentcostmetertype", boost::bind(&CWebServer::SetCurrentCostUSBType, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("restoredatabase", boost::bind(&CWebServer::RestoreDatabase, this, _1, _2, _3));
			m_pWebEm->RegisterActionCode("sbfspotimportolddata", boost::bind(&CWebServer::SBFSpotImportOldData, this, _1, _2, _3));

			m_pWebEm->RegisterActionCode("event_create", boost::bind(&CWebServer::EventCreate, this, _1, _2, _3));

			RegisterCommandCode("getlanguage", boost::bind(&CWebServer::Cmd_GetLanguage, this, _1, _2, _3), true);
			RegisterCommandCode("getthemes", boost::bind(&CWebServer::Cmd_GetThemes, this, _1, _2, _3), true);
			RegisterCommandCode("gettitle", boost::bind(&CWebServer::Cmd_GetTitle, this, _1, _2, _3), true);

			RegisterCommandCode("logincheck", boost::bind(&CWebServer::Cmd_LoginCheck, this, _1, _2, _3), true);
			RegisterCommandCode("getversion", boost::bind(&CWebServer::Cmd_GetVersion, this, _1, _2, _3), true);
			RegisterCommandCode("getlog", boost::bind(&CWebServer::Cmd_GetLog, this, _1, _2, _3));
			RegisterCommandCode("clearlog", boost::bind(&CWebServer::Cmd_ClearLog, this, _1, _2, _3));
			RegisterCommandCode("getauth", boost::bind(&CWebServer::Cmd_GetAuth, this, _1, _2, _3), true);
			RegisterCommandCode("getuptime", boost::bind(&CWebServer::Cmd_GetUptime, this, _1, _2, _3), true);


			RegisterCommandCode("gethardwaretypes", boost::bind(&CWebServer::Cmd_GetHardwareTypes, this, _1, _2, _3));
			RegisterCommandCode("addhardware", boost::bind(&CWebServer::Cmd_AddHardware, this, _1, _2, _3));
			RegisterCommandCode("updatehardware", boost::bind(&CWebServer::Cmd_UpdateHardware, this, _1, _2, _3));
			RegisterCommandCode("deletehardware", boost::bind(&CWebServer::Cmd_DeleteHardware, this, _1, _2, _3));

			RegisterCommandCode("addcamera", boost::bind(&CWebServer::Cmd_AddCamera, this, _1, _2, _3));
			RegisterCommandCode("updatecamera", boost::bind(&CWebServer::Cmd_UpdateCamera, this, _1, _2, _3));
			RegisterCommandCode("deletecamera", boost::bind(&CWebServer::Cmd_DeleteCamera, this, _1, _2, _3));

			RegisterCommandCode("wolgetnodes", boost::bind(&CWebServer::Cmd_WOLGetNodes, this, _1, _2, _3));
			RegisterCommandCode("woladdnode", boost::bind(&CWebServer::Cmd_WOLAddNode, this, _1, _2, _3));
			RegisterCommandCode("wolupdatenode", boost::bind(&CWebServer::Cmd_WOLUpdateNode, this, _1, _2, _3));
			RegisterCommandCode("wolremovenode", boost::bind(&CWebServer::Cmd_WOLRemoveNode, this, _1, _2, _3));
			RegisterCommandCode("wolclearnodes", boost::bind(&CWebServer::Cmd_WOLClearNodes, this, _1, _2, _3));

			RegisterCommandCode("mysensorsgetnodes", boost::bind(&CWebServer::Cmd_MySensorsGetNodes, this, _1, _2, _3));
			RegisterCommandCode("mysensorsgetchilds", boost::bind(&CWebServer::Cmd_MySensorsGetChilds, this, _1, _2, _3));
			RegisterCommandCode("mysensorsupdatenode", boost::bind(&CWebServer::Cmd_MySensorsUpdateNode, this, _1, _2, _3));
			RegisterCommandCode("mysensorsremovenode", boost::bind(&CWebServer::Cmd_MySensorsRemoveNode, this, _1, _2, _3));
			RegisterCommandCode("mysensorsremovechild", boost::bind(&CWebServer::Cmd_MySensorsRemoveChild, this, _1, _2, _3));
			RegisterCommandCode("mysensorsupdatechild", boost::bind(&CWebServer::Cmd_MySensorsUpdateChild, this, _1, _2, _3));

			RegisterCommandCode("pingersetmode", boost::bind(&CWebServer::Cmd_PingerSetMode, this, _1, _2, _3));
			RegisterCommandCode("pingergetnodes", boost::bind(&CWebServer::Cmd_PingerGetNodes, this, _1, _2, _3));
			RegisterCommandCode("pingeraddnode", boost::bind(&CWebServer::Cmd_PingerAddNode, this, _1, _2, _3));
			RegisterCommandCode("pingerupdatenode", boost::bind(&CWebServer::Cmd_PingerUpdateNode, this, _1, _2, _3));
			RegisterCommandCode("pingerremovenode", boost::bind(&CWebServer::Cmd_PingerRemoveNode, this, _1, _2, _3));
			RegisterCommandCode("pingerclearnodes", boost::bind(&CWebServer::Cmd_PingerClearNodes, this, _1, _2, _3));

			RegisterCommandCode("kodisetmode", boost::bind(&CWebServer::Cmd_KodiSetMode, this, _1, _2, _3));
			RegisterCommandCode("kodigetnodes", boost::bind(&CWebServer::Cmd_KodiGetNodes, this, _1, _2, _3));
			RegisterCommandCode("kodiaddnode", boost::bind(&CWebServer::Cmd_KodiAddNode, this, _1, _2, _3));
			RegisterCommandCode("kodiupdatenode", boost::bind(&CWebServer::Cmd_KodiUpdateNode, this, _1, _2, _3));
			RegisterCommandCode("kodiremovenode", boost::bind(&CWebServer::Cmd_KodiRemoveNode, this, _1, _2, _3));
			RegisterCommandCode("kodiclearnodes", boost::bind(&CWebServer::Cmd_KodiClearNodes, this, _1, _2, _3));
			RegisterCommandCode("kodimediacommand", boost::bind(&CWebServer::Cmd_KodiMediaCommand, this, _1, _2, _3));

			RegisterCommandCode("panasonicsetmode", boost::bind(&CWebServer::Cmd_PanasonicSetMode, this, _1, _2, _3));
			RegisterCommandCode("panasonicgetnodes", boost::bind(&CWebServer::Cmd_PanasonicGetNodes, this, _1, _2, _3));
			RegisterCommandCode("panasonicaddnode", boost::bind(&CWebServer::Cmd_PanasonicAddNode, this, _1, _2, _3));
			RegisterCommandCode("panasonicupdatenode", boost::bind(&CWebServer::Cmd_PanasonicUpdateNode, this, _1, _2, _3));
			RegisterCommandCode("panasonicremovenode", boost::bind(&CWebServer::Cmd_PanasonicRemoveNode, this, _1, _2, _3));
			RegisterCommandCode("panasonicclearnodes", boost::bind(&CWebServer::Cmd_PanasonicClearNodes, this, _1, _2, _3));
			RegisterCommandCode("panasonicmediacommand", boost::bind(&CWebServer::Cmd_PanasonicMediaCommand, this, _1, _2, _3));

			RegisterCommandCode("heossetmode", boost::bind(&CWebServer::Cmd_HEOSSetMode, this, _1, _2, _3));
			RegisterCommandCode("heosmediacommand", boost::bind(&CWebServer::Cmd_HEOSMediaCommand, this, _1, _2, _3));

			RegisterCommandCode("onkyoeiscpcommand", boost::bind(&CWebServer::Cmd_OnkyoEiscpCommand, this, _1, _2, _3));

			RegisterCommandCode("bleboxsetmode", boost::bind(&CWebServer::Cmd_BleBoxSetMode, this, _1, _2, _3));
			RegisterCommandCode("bleboxgetnodes", boost::bind(&CWebServer::Cmd_BleBoxGetNodes, this, _1, _2, _3));
			RegisterCommandCode("bleboxaddnode", boost::bind(&CWebServer::Cmd_BleBoxAddNode, this, _1, _2, _3));
			RegisterCommandCode("bleboxremovenode", boost::bind(&CWebServer::Cmd_BleBoxRemoveNode, this, _1, _2, _3));
			RegisterCommandCode("bleboxclearnodes", boost::bind(&CWebServer::Cmd_BleBoxClearNodes, this, _1, _2, _3));
			RegisterCommandCode("bleboxautosearchingnodes", boost::bind(&CWebServer::Cmd_BleBoxAutoSearchingNodes, this, _1, _2, _3));
			RegisterCommandCode("bleboxupdatefirmware", boost::bind(&CWebServer::Cmd_BleBoxUpdateFirmware, this, _1, _2, _3));

			RegisterCommandCode("lmssetmode", boost::bind(&CWebServer::Cmd_LMSSetMode, this, _1, _2, _3));
			RegisterCommandCode("lmsgetnodes", boost::bind(&CWebServer::Cmd_LMSGetNodes, this, _1, _2, _3));
			RegisterCommandCode("lmsgetplaylists", boost::bind(&CWebServer::Cmd_LMSGetPlaylists, this, _1, _2, _3));
			RegisterCommandCode("lmsmediacommand", boost::bind(&CWebServer::Cmd_LMSMediaCommand, this, _1, _2, _3));
			RegisterCommandCode("lmsdeleteunuseddevices", boost::bind(&CWebServer::Cmd_LMSDeleteUnusedDevices, this, _1, _2, _3));

			RegisterCommandCode("savefibarolinkconfig", boost::bind(&CWebServer::Cmd_SaveFibaroLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("getfibarolinkconfig", boost::bind(&CWebServer::Cmd_GetFibaroLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("getfibarolinks", boost::bind(&CWebServer::Cmd_GetFibaroLinks, this, _1, _2, _3));
			RegisterCommandCode("savefibarolink", boost::bind(&CWebServer::Cmd_SaveFibaroLink, this, _1, _2, _3));
			RegisterCommandCode("deletefibarolink", boost::bind(&CWebServer::Cmd_DeleteFibaroLink, this, _1, _2, _3));

			RegisterCommandCode("saveinfluxlinkconfig", boost::bind(&CWebServer::Cmd_SaveInfluxLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("getinfluxlinkconfig", boost::bind(&CWebServer::Cmd_GetInfluxLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("getinfluxlinks", boost::bind(&CWebServer::Cmd_GetInfluxLinks, this, _1, _2, _3));
			RegisterCommandCode("saveinfluxlink", boost::bind(&CWebServer::Cmd_SaveInfluxLink, this, _1, _2, _3));
			RegisterCommandCode("deleteinfluxlink", boost::bind(&CWebServer::Cmd_DeleteInfluxLink, this, _1, _2, _3));

			RegisterCommandCode("savehttplinkconfig", boost::bind(&CWebServer::Cmd_SaveHttpLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("gethttplinkconfig", boost::bind(&CWebServer::Cmd_GetHttpLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("gethttplinks", boost::bind(&CWebServer::Cmd_GetHttpLinks, this, _1, _2, _3));
			RegisterCommandCode("savehttplink", boost::bind(&CWebServer::Cmd_SaveHttpLink, this, _1, _2, _3));
			RegisterCommandCode("deletehttplink", boost::bind(&CWebServer::Cmd_DeleteHttpLink, this, _1, _2, _3));

			RegisterCommandCode("savegooglepubsublinkconfig", boost::bind(&CWebServer::Cmd_SaveGooglePubSubLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("getgooglepubsublinkconfig", boost::bind(&CWebServer::Cmd_GetGooglePubSubLinkConfig, this, _1, _2, _3));
			RegisterCommandCode("getgooglepubsublinks", boost::bind(&CWebServer::Cmd_GetGooglePubSubLinks, this, _1, _2, _3));
			RegisterCommandCode("savegooglepubsublink", boost::bind(&CWebServer::Cmd_SaveGooglePubSubLink, this, _1, _2, _3));
			RegisterCommandCode("deletegooglepubsublink", boost::bind(&CWebServer::Cmd_DeleteGooglePubSubLink, this, _1, _2, _3));

			RegisterCommandCode("getdevicevalueoptions", boost::bind(&CWebServer::Cmd_GetDeviceValueOptions, this, _1, _2, _3));
			RegisterCommandCode("getdevicevalueoptionwording", boost::bind(&CWebServer::Cmd_GetDeviceValueOptionWording, this, _1, _2, _3));

			RegisterCommandCode("adduservariable", boost::bind(&CWebServer::Cmd_AddUserVariable, this, _1, _2, _3));
			RegisterCommandCode("updateuservariable", boost::bind(&CWebServer::Cmd_UpdateUserVariable, this, _1, _2, _3));
			RegisterCommandCode("deleteuservariable", boost::bind(&CWebServer::Cmd_DeleteUserVariable, this, _1, _2, _3));
			RegisterCommandCode("getuservariables", boost::bind(&CWebServer::Cmd_GetUserVariables, this, _1, _2, _3));
			RegisterCommandCode("getuservariable", boost::bind(&CWebServer::Cmd_GetUserVariable, this, _1, _2, _3));

			RegisterCommandCode("allownewhardware", boost::bind(&CWebServer::Cmd_AllowNewHardware, this, _1, _2, _3));

			RegisterCommandCode("addplan", boost::bind(&CWebServer::Cmd_AddPlan, this, _1, _2, _3));
			RegisterCommandCode("updateplan", boost::bind(&CWebServer::Cmd_UpdatePlan, this, _1, _2, _3));
			RegisterCommandCode("deleteplan", boost::bind(&CWebServer::Cmd_DeletePlan, this, _1, _2, _3));
			RegisterCommandCode("getunusedplandevices", boost::bind(&CWebServer::Cmd_GetUnusedPlanDevices, this, _1, _2, _3));
			RegisterCommandCode("addplanactivedevice", boost::bind(&CWebServer::Cmd_AddPlanActiveDevice, this, _1, _2, _3));
			RegisterCommandCode("getplandevices", boost::bind(&CWebServer::Cmd_GetPlanDevices, this, _1, _2, _3));
			RegisterCommandCode("deleteplandevice", boost::bind(&CWebServer::Cmd_DeletePlanDevice, this, _1, _2, _3));
			RegisterCommandCode("setplandevicecoords", boost::bind(&CWebServer::Cmd_SetPlanDeviceCoords, this, _1, _2, _3));
			RegisterCommandCode("deleteallplandevices", boost::bind(&CWebServer::Cmd_DeleteAllPlanDevices, this, _1, _2, _3));
			RegisterCommandCode("changeplanorder", boost::bind(&CWebServer::Cmd_ChangePlanOrder, this, _1, _2, _3));
			RegisterCommandCode("changeplandeviceorder", boost::bind(&CWebServer::Cmd_ChangePlanDeviceOrder, this, _1, _2, _3));

			RegisterCommandCode("gettimerplans", boost::bind(&CWebServer::Cmd_GetTimerPlans, this, _1, _2, _3));
			RegisterCommandCode("addtimerplan", boost::bind(&CWebServer::Cmd_AddTimerPlan, this, _1, _2, _3));
			RegisterCommandCode("updatetimerplan", boost::bind(&CWebServer::Cmd_UpdateTimerPlan, this, _1, _2, _3));
			RegisterCommandCode("deletetimerplan", boost::bind(&CWebServer::Cmd_DeleteTimerPlan, this, _1, _2, _3));
			RegisterCommandCode("duplicatetimerplan", boost::bind(&CWebServer::Cmd_DuplicateTimerPlan, this, _1, _2, _3));

			RegisterCommandCode("getactualhistory", boost::bind(&CWebServer::Cmd_GetActualHistory, this, _1, _2, _3));
			RegisterCommandCode("getnewhistory", boost::bind(&CWebServer::Cmd_GetNewHistory, this, _1, _2, _3));

			RegisterCommandCode("getconfig", boost::bind(&CWebServer::Cmd_GetConfig, this, _1, _2, _3), true);
			RegisterCommandCode("sendnotification", boost::bind(&CWebServer::Cmd_SendNotification, this, _1, _2, _3));
			RegisterCommandCode("emailcamerasnapshot", boost::bind(&CWebServer::Cmd_EmailCameraSnapshot, this, _1, _2, _3));
			RegisterCommandCode("udevice", boost::bind(&CWebServer::Cmd_UpdateDevice, this, _1, _2, _3));
			RegisterCommandCode("udevices", boost::bind(&CWebServer::Cmd_UpdateDevices, this, _1, _2, _3));
			RegisterCommandCode("thermostatstate", boost::bind(&CWebServer::Cmd_SetThermostatState, this, _1, _2, _3));
			RegisterCommandCode("system_shutdown", boost::bind(&CWebServer::Cmd_SystemShutdown, this, _1, _2, _3));
			RegisterCommandCode("system_reboot", boost::bind(&CWebServer::Cmd_SystemReboot, this, _1, _2, _3));
			RegisterCommandCode("execute_script", boost::bind(&CWebServer::Cmd_ExcecuteScript, this, _1, _2, _3));
			RegisterCommandCode("getcosts", boost::bind(&CWebServer::Cmd_GetCosts, this, _1, _2, _3));
			RegisterCommandCode("checkforupdate", boost::bind(&CWebServer::Cmd_CheckForUpdate, this, _1, _2, _3));
			RegisterCommandCode("downloadupdate", boost::bind(&CWebServer::Cmd_DownloadUpdate, this, _1, _2, _3));
			RegisterCommandCode("downloadready", boost::bind(&CWebServer::Cmd_DownloadReady, this, _1, _2, _3));
			RegisterCommandCode("deletedatapoint", boost::bind(&CWebServer::Cmd_DeleteDatePoint, this, _1, _2, _3));

			RegisterCommandCode("setactivetimerplan", boost::bind(&CWebServer::Cmd_SetActiveTimerPlan, this, _1, _2, _3));
			RegisterCommandCode("addtimer", boost::bind(&CWebServer::Cmd_AddTimer, this, _1, _2, _3));
			RegisterCommandCode("updatetimer", boost::bind(&CWebServer::Cmd_UpdateTimer, this, _1, _2, _3));
			RegisterCommandCode("deletetimer", boost::bind(&CWebServer::Cmd_DeleteTimer, this, _1, _2, _3));
			RegisterCommandCode("enabletimer", boost::bind(&CWebServer::Cmd_EnableTimer, this, _1, _2, _3));
			RegisterCommandCode("disabletimer", boost::bind(&CWebServer::Cmd_DisableTimer, this, _1, _2, _3));
			RegisterCommandCode("cleartimers", boost::bind(&CWebServer::Cmd_ClearTimers, this, _1, _2, _3));

			RegisterCommandCode("addscenetimer", boost::bind(&CWebServer::Cmd_AddSceneTimer, this, _1, _2, _3));
			RegisterCommandCode("updatescenetimer", boost::bind(&CWebServer::Cmd_UpdateSceneTimer, this, _1, _2, _3));
			RegisterCommandCode("deletescenetimer", boost::bind(&CWebServer::Cmd_DeleteSceneTimer, this, _1, _2, _3));
			RegisterCommandCode("enablescenetimer", boost::bind(&CWebServer::Cmd_EnableSceneTimer, this, _1, _2, _3));
			RegisterCommandCode("disablescenetimer", boost::bind(&CWebServer::Cmd_DisableSceneTimer, this, _1, _2, _3));
			RegisterCommandCode("clearscenetimers", boost::bind(&CWebServer::Cmd_ClearSceneTimers, this, _1, _2, _3));
			RegisterCommandCode("getsceneactivations", boost::bind(&CWebServer::Cmd_GetSceneActivations, this, _1, _2, _3));
			RegisterCommandCode("addscenecode", boost::bind(&CWebServer::Cmd_AddSceneCode, this, _1, _2, _3));
			RegisterCommandCode("removescenecode", boost::bind(&CWebServer::Cmd_RemoveSceneCode, this, _1, _2, _3));
			RegisterCommandCode("clearscenecodes", boost::bind(&CWebServer::Cmd_ClearSceneCodes, this, _1, _2, _3));
			RegisterCommandCode("renamescene", boost::bind(&CWebServer::Cmd_RenameScene, this, _1, _2, _3));

			RegisterCommandCode("setsetpoint", boost::bind(&CWebServer::Cmd_SetSetpoint, this, _1, _2, _3));
			RegisterCommandCode("addsetpointtimer", boost::bind(&CWebServer::Cmd_AddSetpointTimer, this, _1, _2, _3));
			RegisterCommandCode("updatesetpointtimer", boost::bind(&CWebServer::Cmd_UpdateSetpointTimer, this, _1, _2, _3));
			RegisterCommandCode("deletesetpointtimer", boost::bind(&CWebServer::Cmd_DeleteSetpointTimer, this, _1, _2, _3));
			RegisterCommandCode("enablesetpointtimer", boost::bind(&CWebServer::Cmd_EnableSetpointTimer, this, _1, _2, _3));
			RegisterCommandCode("disablesetpointtimer", boost::bind(&CWebServer::Cmd_DisableSetpointTimer, this, _1, _2, _3));
			RegisterCommandCode("clearsetpointtimers", boost::bind(&CWebServer::Cmd_ClearSetpointTimers, this, _1, _2, _3));

			RegisterCommandCode("serial_devices", boost::bind(&CWebServer::Cmd_GetSerialDevices, this, _1, _2, _3));
			RegisterCommandCode("devices_list", boost::bind(&CWebServer::Cmd_GetDevicesList, this, _1, _2, _3));
			RegisterCommandCode("devices_list_onoff", boost::bind(&CWebServer::Cmd_GetDevicesListOnOff, this, _1, _2, _3));

			RegisterCommandCode("registerhue", boost::bind(&CWebServer::Cmd_PhilipsHueRegister, this, _1, _2, _3));

			RegisterCommandCode("getcustomiconset", boost::bind(&CWebServer::Cmd_GetCustomIconSet, this, _1, _2, _3));
			RegisterCommandCode("deletecustomicon", boost::bind(&CWebServer::Cmd_DeleteCustomIcon, this, _1, _2, _3));
			RegisterCommandCode("updatecustomicon", boost::bind(&CWebServer::Cmd_UpdateCustomIcon, this, _1, _2, _3));

			RegisterCommandCode("renamedevice", boost::bind(&CWebServer::Cmd_RenameDevice, this, _1, _2, _3));
			RegisterCommandCode("setunused", boost::bind(&CWebServer::Cmd_SetUnused, this, _1, _2, _3));

			RegisterCommandCode("addlogmessage", boost::bind(&CWebServer::Cmd_AddLogMessage, this, _1, _2, _3));
			RegisterCommandCode("clearshortlog", boost::bind(&CWebServer::Cmd_ClearShortLog, this, _1, _2, _3));
			RegisterCommandCode("vacuumdatabase", boost::bind(&CWebServer::Cmd_VacuumDatabase, this, _1, _2, _3));

			RegisterCommandCode("addmobiledevice", boost::bind(&CWebServer::Cmd_AddMobileDevice, this, _1, _2, _3));
			RegisterCommandCode("updatemobiledevice", boost::bind(&CWebServer::Cmd_UpdateMobileDevice, this, _1, _2, _3));
			RegisterCommandCode("deletemobiledevice", boost::bind(&CWebServer::Cmd_DeleteMobileDevice, this, _1, _2, _3));

			RegisterCommandCode("addyeelight", boost::bind(&CWebServer::Cmd_AddYeeLight, this, _1, _2, _3));

			RegisterCommandCode("addArilux", boost::bind(&CWebServer::Cmd_AddArilux, this, _1, _2, _3));

			RegisterRType("graph", boost::bind(&CWebServer::RType_HandleGraph, this, _1, _2, _3));
			RegisterRType("lightlog", boost::bind(&CWebServer::RType_LightLog, this, _1, _2, _3));
			RegisterRType("textlog", boost::bind(&CWebServer::RType_TextLog, this, _1, _2, _3));
			RegisterRType("scenelog", boost::bind(&CWebServer::RType_SceneLog, this, _1, _2, _3));
			RegisterRType("settings", boost::bind(&CWebServer::RType_Settings, this, _1, _2, _3));
			RegisterRType("events", boost::bind(&CWebServer::RType_Events, this, _1, _2, _3));

			RegisterRType("hardware", boost::bind(&CWebServer::RType_Hardware, this, _1, _2, _3));
			RegisterRType("devices", boost::bind(&CWebServer::RType_Devices, this, _1, _2, _3));
			RegisterRType("deletedevice", boost::bind(&CWebServer::RType_DeleteDevice, this, _1, _2, _3));
			RegisterRType("cameras", boost::bind(&CWebServer::RType_Cameras, this, _1, _2, _3));
			RegisterRType("cameras_user", boost::bind(&CWebServer::RType_CamerasUser, this, _1, _2, _3));
			RegisterRType("users", boost::bind(&CWebServer::RType_Users, this, _1, _2, _3));
			RegisterRType("mobiles", boost::bind(&CWebServer::RType_Mobiles, this, _1, _2, _3));

			RegisterRType("timers", boost::bind(&CWebServer::RType_Timers, this, _1, _2, _3));
			RegisterRType("scenetimers", boost::bind(&CWebServer::RType_SceneTimers, this, _1, _2, _3));
			RegisterRType("setpointtimers", boost::bind(&CWebServer::RType_SetpointTimers, this, _1, _2, _3));

			RegisterRType("gettransfers", boost::bind(&CWebServer::RType_GetTransfers, this, _1, _2, _3));
			RegisterRType("transferdevice", boost::bind(&CWebServer::RType_TransferDevice, this, _1, _2, _3));
			RegisterRType("notifications", boost::bind(&CWebServer::RType_Notifications, this, _1, _2, _3));
			RegisterRType("schedules", boost::bind(&CWebServer::RType_Schedules, this, _1, _2, _3));
			RegisterRType("getshareduserdevices", boost::bind(&CWebServer::RType_GetSharedUserDevices, this, _1, _2, _3));
			RegisterRType("setshareduserdevices", boost::bind(&CWebServer::RType_SetSharedUserDevices, this, _1, _2, _3));
			RegisterRType("setused", boost::bind(&CWebServer::RType_SetUsed, this, _1, _2, _3));
			RegisterRType("scenes", boost::bind(&CWebServer::RType_Scenes, this, _1, _2, _3));
			RegisterRType("addscene", boost::bind(&CWebServer::RType_AddScene, this, _1, _2, _3));
			RegisterRType("deletescene", boost::bind(&CWebServer::RType_DeleteScene, this, _1, _2, _3));
			RegisterRType("updatescene", boost::bind(&CWebServer::RType_UpdateScene, this, _1, _2, _3));
			RegisterRType("createvirtualsensor", boost::bind(&CWebServer::RType_CreateMappedSensor, this, _1, _2, _3));
			RegisterRType("createdevice", boost::bind(&CWebServer::RType_CreateDevice, this, _1, _2, _3));

			RegisterRType("createevohomesensor", boost::bind(&CWebServer::RType_CreateEvohomeSensor, this, _1, _2, _3));
			RegisterRType("bindevohome", boost::bind(&CWebServer::RType_BindEvohome, this, _1, _2, _3));
			RegisterRType("createrflinkdevice", boost::bind(&CWebServer::RType_CreateRFLinkDevice, this, _1, _2, _3));

			RegisterRType("custom_light_icons", boost::bind(&CWebServer::RType_CustomLightIcons, this, _1, _2, _3));
			RegisterRType("plans", boost::bind(&CWebServer::RType_Plans, this, _1, _2, _3));
			RegisterRType("floorplans", boost::bind(&CWebServer::RType_FloorPlans, this, _1, _2, _3));
#ifdef WITH_OPENZWAVE
			//ZWave
			RegisterCommandCode("updatezwavenode", boost::bind(&CWebServer::Cmd_ZWaveUpdateNode, this, _1, _2, _3));
			RegisterCommandCode("deletezwavenode", boost::bind(&CWebServer::Cmd_ZWaveDeleteNode, this, _1, _2, _3));
			RegisterCommandCode("zwaveinclude", boost::bind(&CWebServer::Cmd_ZWaveInclude, this, _1, _2, _3));
			RegisterCommandCode("zwaveexclude", boost::bind(&CWebServer::Cmd_ZWaveExclude, this, _1, _2, _3));

			RegisterCommandCode("zwaveisnodeincluded", boost::bind(&CWebServer::Cmd_ZWaveIsNodeIncluded, this, _1, _2, _3));
			RegisterCommandCode("zwaveisnodeexcluded", boost::bind(&CWebServer::Cmd_ZWaveIsNodeExcluded, this, _1, _2, _3));

			RegisterCommandCode("zwavesoftreset", boost::bind(&CWebServer::Cmd_ZWaveSoftReset, this, _1, _2, _3));
			RegisterCommandCode("zwavehardreset", boost::bind(&CWebServer::Cmd_ZWaveHardReset, this, _1, _2, _3));
			RegisterCommandCode("zwavenetworkheal", boost::bind(&CWebServer::Cmd_ZWaveNetworkHeal, this, _1, _2, _3));
			RegisterCommandCode("zwavenodeheal", boost::bind(&CWebServer::Cmd_ZWaveNodeHeal, this, _1, _2, _3));
			RegisterCommandCode("zwavenetworkinfo", boost::bind(&CWebServer::Cmd_ZWaveNetworkInfo, this, _1, _2, _3));
			RegisterCommandCode("zwaveremovegroupnode", boost::bind(&CWebServer::Cmd_ZWaveRemoveGroupNode, this, _1, _2, _3));
			RegisterCommandCode("zwaveaddgroupnode", boost::bind(&CWebServer::Cmd_ZWaveAddGroupNode, this, _1, _2, _3));
			RegisterCommandCode("zwavegroupinfo", boost::bind(&CWebServer::Cmd_ZWaveGroupInfo, this, _1, _2, _3));
			RegisterCommandCode("zwavecancel", boost::bind(&CWebServer::Cmd_ZWaveCancel, this, _1, _2, _3));
			RegisterCommandCode("applyzwavenodeconfig", boost::bind(&CWebServer::Cmd_ApplyZWaveNodeConfig, this, _1, _2, _3));
			RegisterCommandCode("requestzwavenodeconfig", boost::bind(&CWebServer::Cmd_ZWaveRequestNodeConfig, this, _1, _2, _3));
			RegisterCommandCode("zwavestatecheck", boost::bind(&CWebServer::Cmd_ZWaveStateCheck, this, _1, _2, _3));
			RegisterCommandCode("zwavereceiveconfigurationfromothercontroller", boost::bind(&CWebServer::Cmd_ZWaveReceiveConfigurationFromOtherController, this, _1, _2, _3));
			RegisterCommandCode("zwavesendconfigurationtosecondcontroller", boost::bind(&CWebServer::Cmd_ZWaveSendConfigurationToSecondaryController, this, _1, _2, _3));
			RegisterCommandCode("zwavetransferprimaryrole", boost::bind(&CWebServer::Cmd_ZWaveTransferPrimaryRole, this, _1, _2, _3));
			RegisterCommandCode("zwavestartusercodeenrollmentmode", boost::bind(&CWebServer::Cmd_ZWaveSetUserCodeEnrollmentMode, this, _1, _2, _3));
			RegisterCommandCode("zwavegetusercodes", boost::bind(&CWebServer::Cmd_ZWaveGetNodeUserCodes, this, _1, _2, _3));
			RegisterCommandCode("zwaveremoveusercode", boost::bind(&CWebServer::Cmd_ZWaveRemoveUserCode, this, _1, _2, _3));

			m_pWebEm->RegisterPageCode("/zwavegetconfig.php", boost::bind(&CWebServer::ZWaveGetConfigFile, this, _1, _2, _3));

			m_pWebEm->RegisterPageCode("/ozwcp/poll.xml", boost::bind(&CWebServer::ZWaveCPPollXml, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/cp.html", boost::bind(&CWebServer::ZWaveCPIndex, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/confparmpost.html", boost::bind(&CWebServer::ZWaveCPNodeGetConf, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/refreshpost.html", boost::bind(&CWebServer::ZWaveCPNodeGetValues, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/valuepost.html", boost::bind(&CWebServer::ZWaveCPNodeSetValue, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/buttonpost.html", boost::bind(&CWebServer::ZWaveCPNodeSetButton, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/admpost.html", boost::bind(&CWebServer::ZWaveCPAdminCommand, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/nodepost.html", boost::bind(&CWebServer::ZWaveCPNodeChange, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/savepost.html", boost::bind(&CWebServer::ZWaveCPSaveConfig, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/thpost.html", boost::bind(&CWebServer::ZWaveCPTestHeal, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/topopost.html", boost::bind(&CWebServer::ZWaveCPGetTopo, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/statpost.html", boost::bind(&CWebServer::ZWaveCPGetStats, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/grouppost.html", boost::bind(&CWebServer::ZWaveCPSetGroup, this, _1, _2, _3));
			m_pWebEm->RegisterPageCode("/ozwcp/scenepost.html", boost::bind(&CWebServer::ZWaveCPSceneCommand, this, _1, _2, _3));
			//
			//pollpost.html
			//scenepost.html
			//thpost.html
			RegisterRType("openzwavenodes", boost::bind(&CWebServer::RType_OpenZWaveNodes, this, _1, _2, _3));
#endif
			RegisterCommandCode("tellstickApplySettings", boost::bind(&CWebServer::Cmd_TellstickApplySettings, this, _1, _2, _3));

			m_pWebEm->RegisterWhitelistURLString("/html5.appcache");
			m_pWebEm->RegisterWhitelistURLString("/images/floorplans/plan");

			//Start normal worker thread
			m_bDoStop = false;
			m_thread = std::make_shared<std::thread>(&CWebServer::Do_Work, this);
			std::string server_name = "WebServer_" + settings.listening_port;
			SetThreadName(m_thread->native_handle(), server_name.c_str());
			return (m_thread != nullptr);
		}