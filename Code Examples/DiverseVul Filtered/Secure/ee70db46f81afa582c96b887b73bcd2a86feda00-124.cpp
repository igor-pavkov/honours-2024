		void CWebServer::SetAuthenticationMethod(const _eAuthenticationMethod amethod)
		{
			if (m_pWebEm == NULL)
				return;
			m_pWebEm->SetAuthenticationMethod(amethod);
		}