	bool SettingsPage(CWebSock& WebSock, CTemplate& Tmpl) {
		Tmpl.SetFile("settings.tmpl");
		if (!WebSock.GetParam("submitted").ToUInt()) {
			CString sBindHosts, sMotd;
			Tmpl["Action"] = "settings";
			Tmpl["Title"] = "Settings";
			Tmpl["StatusPrefix"] = CZNC::Get().GetStatusPrefix();
			Tmpl["MaxBufferSize"] = CString(CZNC::Get().GetMaxBufferSize());
			Tmpl["ConnectDelay"] = CString(CZNC::Get().GetConnectDelay());
			Tmpl["ServerThrottle"] = CString(CZNC::Get().GetServerThrottle());
			Tmpl["AnonIPLimit"] = CString(CZNC::Get().GetAnonIPLimit());
			Tmpl["ProtectWebSessions"] = CString(CZNC::Get().GetProtectWebSessions());

			const VCString& vsBindHosts = CZNC::Get().GetBindHosts();
			for (unsigned int a = 0; a < vsBindHosts.size(); a++) {
				CTemplate& l = Tmpl.AddRow("BindHostLoop");
				l["BindHost"] = vsBindHosts[a];
			}

			const VCString& vsMotd = CZNC::Get().GetMotd();
			for (unsigned int b = 0; b < vsMotd.size(); b++) {
				CTemplate& l = Tmpl.AddRow("MOTDLoop");
				l["Line"] = vsMotd[b];
			}

			const vector<CListener*>& vpListeners = CZNC::Get().GetListeners();
			for (unsigned int c = 0; c < vpListeners.size(); c++) {
				CListener* pListener = vpListeners[c];
				CTemplate& l = Tmpl.AddRow("ListenLoop");

				l["Port"] = CString(pListener->GetPort());
				l["BindHost"] = pListener->GetBindHost();

				l["IsWeb"] = CString(pListener->GetAcceptType() != CListener::ACCEPT_IRC);
				l["IsIRC"] = CString(pListener->GetAcceptType() != CListener::ACCEPT_HTTP);

				// simple protection for user from shooting his own foot
				// TODO check also for hosts/families
				// such check is only here, user still can forge HTTP request to delete web port
				l["SuggestDeletion"] = CString(pListener->GetPort() != WebSock.GetLocalPort());

#ifdef HAVE_LIBSSL
				if (pListener->IsSSL()) {
					l["IsSSL"] = "true";
				}
#endif

#ifdef HAVE_IPV6
				switch (pListener->GetAddrType()) {
					case ADDR_IPV4ONLY:
						l["IsIPV4"] = "true";
						break;
					case ADDR_IPV6ONLY:
						l["IsIPV6"] = "true";
						break;
					case ADDR_ALL:
						l["IsIPV4"] = "true";
						l["IsIPV6"] = "true";
						break;
				}
#else
				l["IsIPV4"] = "true";
#endif
			}

			vector<CString> vDirs;
			WebSock.GetAvailSkins(vDirs);

			for (unsigned int d = 0; d < vDirs.size(); d++) {
				const CString& SubDir = vDirs[d];
				CTemplate& l = Tmpl.AddRow("SkinLoop");
				l["Name"] = SubDir;

				if (SubDir == CZNC::Get().GetSkinName()) {
					l["Checked"] = "true";
				}
			}

			set<CModInfo> ssGlobalMods;
			CZNC::Get().GetModules().GetAvailableMods(ssGlobalMods, CModInfo::GlobalModule);

			for (set<CModInfo>::iterator it = ssGlobalMods.begin(); it != ssGlobalMods.end(); ++it) {
				const CModInfo& Info = *it;
				CTemplate& l = Tmpl.AddRow("ModuleLoop");

				CModule *pModule = CZNC::Get().GetModules().FindModule(Info.GetName());
				if (pModule) {
					l["Checked"] = "true";
					l["Args"] = pModule->GetArgs();
					if (CModInfo::GlobalModule == GetType() && Info.GetName() == GetModName()) {
						l["Disabled"] = "true";
					}
				}

				l["Name"] = Info.GetName();
				l["Description"] = Info.GetDescription();
				l["Wiki"] = Info.GetWikiPage();
				l["HasArgs"] = CString(Info.GetHasArgs());
				l["ArgsHelpText"] = Info.GetArgsHelpText();
			}

			return true;
		}

		CString sArg;
		sArg = WebSock.GetParam("statusprefix"); CZNC::Get().SetStatusPrefix(sArg);
		sArg = WebSock.GetParam("maxbufsize"); CZNC::Get().SetMaxBufferSize(sArg.ToUInt());
		sArg = WebSock.GetParam("connectdelay"); CZNC::Get().SetConnectDelay(sArg.ToUInt());
		sArg = WebSock.GetParam("serverthrottle"); CZNC::Get().SetServerThrottle(sArg.ToUInt());
		sArg = WebSock.GetParam("anoniplimit"); CZNC::Get().SetAnonIPLimit(sArg.ToUInt());
		sArg = WebSock.GetParam("protectwebsessions"); CZNC::Get().SetProtectWebSessions(sArg.ToBool());

		VCString vsArgs;
		WebSock.GetRawParam("motd").Split("\n", vsArgs);
		CZNC::Get().ClearMotd();

		unsigned int a = 0;
		for (a = 0; a < vsArgs.size(); a++) {
			CZNC::Get().AddMotd(vsArgs[a].TrimRight_n());
		}

		WebSock.GetRawParam("bindhosts").Split("\n", vsArgs);
		CZNC::Get().ClearBindHosts();

		for (a = 0; a < vsArgs.size(); a++) {
			CZNC::Get().AddBindHost(vsArgs[a].Trim_n());
		}

		CZNC::Get().SetSkinName(WebSock.GetParam("skin"));

		set<CString> ssArgs;
		WebSock.GetParamValues("loadmod", ssArgs);

		for (set<CString>::iterator it = ssArgs.begin(); it != ssArgs.end(); ++it) {
			CString sModRet;
			CString sModName = (*it).TrimRight_n("\r");
			CString sModLoadError;

			if (!sModName.empty()) {
				CString sArgs = WebSock.GetParam("modargs_" + sModName);

				CModule *pMod = CZNC::Get().GetModules().FindModule(sModName);
				if (!pMod) {
					if (!CZNC::Get().GetModules().LoadModule(sModName, sArgs, CModInfo::GlobalModule, NULL, NULL, sModRet)) {
						sModLoadError = "Unable to load module [" + sModName + "] [" + sModRet + "]";
					}
				} else if (pMod->GetArgs() != sArgs) {
					if (!CZNC::Get().GetModules().ReloadModule(sModName, sArgs, NULL, NULL, sModRet)) {
						sModLoadError = "Unable to reload module [" + sModName + "] [" + sModRet + "]";
					}
				}

				if (!sModLoadError.empty()) {
					DEBUG(sModLoadError);
					WebSock.GetSession()->AddError(sModLoadError);
				}
			}
		}

		const CModules& vCurMods = CZNC::Get().GetModules();
		set<CString> ssUnloadMods;

		for (a = 0; a < vCurMods.size(); a++) {
			CModule* pCurMod = vCurMods[a];

			if (ssArgs.find(pCurMod->GetModName()) == ssArgs.end() &&
					(CModInfo::GlobalModule != GetType() || pCurMod->GetModName() != GetModName())) {
				ssUnloadMods.insert(pCurMod->GetModName());
			}
		}

		for (set<CString>::iterator it2 = ssUnloadMods.begin(); it2 != ssUnloadMods.end(); ++it2) {
			CZNC::Get().GetModules().UnloadModule(*it2);
		}

		if (!CZNC::Get().WriteConfig()) {
			WebSock.GetSession()->AddError("Settings changed, but config was not written");
		}

		WebSock.Redirect("settings");
		/* we don't want the template to be printed while we redirect */
		return false;
	}