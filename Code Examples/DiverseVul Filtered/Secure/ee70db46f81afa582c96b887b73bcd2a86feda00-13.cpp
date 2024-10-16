		void CWebServer::SetWebRoot(const std::string &webRoot)
		{
			if (m_pWebEm == NULL)
				return;
			m_pWebEm->SetWebRoot(webRoot);
		}