	bool TrafficPage(CWebSock& WebSock, CTemplate& Tmpl) {
		CSmartPtr<CWebSession> spSession = WebSock.GetSession();
		Tmpl["Uptime"] = CZNC::Get().GetUptime();

		const map<CString,CUser*>& msUsers = CZNC::Get().GetUserMap();
		Tmpl["TotalUsers"] = CString(msUsers.size());

		size_t uiNetworks = 0, uiAttached = 0, uiClients = 0, uiServers = 0;

		for (map<CString,CUser*>::const_iterator it = msUsers.begin(); it != msUsers.end(); ++it) {
			CUser& User = *it->second;
			vector<CIRCNetwork*> vNetworks = User.GetNetworks();

			for (vector<CIRCNetwork*>::const_iterator it2 = vNetworks.begin(); it2 != vNetworks.end(); ++it2) {
				CIRCNetwork *pNetwork = *it2;
				uiNetworks++;

				if (pNetwork->IsIRCConnected()) {
					uiServers++;
				}

				if (pNetwork->IsNetworkAttached()) {
					uiAttached++;
				}

				uiClients += pNetwork->GetClients().size();
			}

			uiClients += User.GetUserClients().size();
		}

		Tmpl["TotalNetworks"] = CString(uiNetworks);
		Tmpl["AttachedNetworks"] = CString(uiAttached);
		Tmpl["TotalCConnections"] = CString(uiClients);
		Tmpl["TotalIRCConnections"] = CString(uiServers);

		CZNC::TrafficStatsPair Users, ZNC, Total;
		CZNC::TrafficStatsMap traffic = CZNC::Get().GetTrafficStats(Users, ZNC, Total);
		CZNC::TrafficStatsMap::const_iterator it;

		for (it = traffic.begin(); it != traffic.end(); ++it) {
			CTemplate& l = Tmpl.AddRow("TrafficLoop");

			l["Username"] = it->first;
			l["In"] = CString::ToByteStr(it->second.first);
			l["Out"] = CString::ToByteStr(it->second.second);
			l["Total"] = CString::ToByteStr(it->second.first + it->second.second);
		}

		Tmpl["UserIn"] = CString::ToByteStr(Users.first);
		Tmpl["UserOut"] = CString::ToByteStr(Users.second);
		Tmpl["UserTotal"] = CString::ToByteStr(Users.first + Users.second);

		Tmpl["ZNCIn"] = CString::ToByteStr(ZNC.first);
		Tmpl["ZNCOut"] = CString::ToByteStr(ZNC.second);
		Tmpl["ZNCTotal"] = CString::ToByteStr(ZNC.first + ZNC.second);

		Tmpl["AllIn"] = CString::ToByteStr(Total.first);
		Tmpl["AllOut"] = CString::ToByteStr(Total.second);
		Tmpl["AllTotal"] = CString::ToByteStr(Total.first + Total.second);

		return true;
	}