		CWebServer::CWebServer(void) : session_store()
		{
			m_pWebEm = NULL;
			m_bDoStop = false;
#ifdef WITH_OPENZWAVE
			m_ZW_Hwidx = -1;
#endif
		}