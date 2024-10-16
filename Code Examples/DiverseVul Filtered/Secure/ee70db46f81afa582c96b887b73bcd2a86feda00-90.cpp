		int CWebServer::FindUser(const char* szUserName)
		{
			int iUser = 0;
			for (const auto & itt : m_users)
			{
				if (itt.Username == szUserName)
					return iUser;
				iUser++;
			}
			return -1;
		}