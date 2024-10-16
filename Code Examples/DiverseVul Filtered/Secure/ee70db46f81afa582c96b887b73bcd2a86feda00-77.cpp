		bool CWebServer::FindAdminUser()
		{
			for (const auto & itt : m_users)
			{
				if (itt.userrights == URIGHTS_ADMIN)
					return true;
			}
			return false;
		}