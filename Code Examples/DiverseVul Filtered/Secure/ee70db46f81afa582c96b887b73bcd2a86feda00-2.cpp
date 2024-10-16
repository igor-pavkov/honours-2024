		void CWebServer::SetWebCompressionMode(const _eWebCompressionMode gzmode)
		{
			if (m_pWebEm == NULL)
				return;
			m_pWebEm->SetWebCompressionMode(gzmode);
		}