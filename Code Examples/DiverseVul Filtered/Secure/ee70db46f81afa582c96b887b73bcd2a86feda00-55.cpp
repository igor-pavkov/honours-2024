		bool CWebServer::IsIdxForUser(const WebEmSession *pSession, const int Idx)
		{
			if (pSession->rights == 2)
				return true;
			if (pSession->rights == 0)
				return false; //viewer
			//User
			int iUser = FindUser(pSession->username.c_str());
			if ((iUser < 0) || (iUser >= (int)m_users.size()))
				return false;

			if (m_users[iUser].TotSensors == 0)
				return true; // all sensors

			std::vector<std::vector<std::string> > result = m_sql.safe_query("SELECT DeviceRowID FROM SharedDevices WHERE (SharedUserID == '%d') AND (DeviceRowID == '%d')", m_users[iUser].ID, Idx);
			return (!result.empty());
		}