		void CWebServer::ClearUserPasswords()
		{
			m_users.clear();
			m_pWebEm->ClearUserPasswords();
		}