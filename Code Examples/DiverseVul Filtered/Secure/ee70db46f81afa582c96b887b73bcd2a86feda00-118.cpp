		void CWebServer::StopServer()
		{
			m_bDoStop = true;
			try
			{
				if (m_pWebEm == NULL)
					return;
				m_pWebEm->Stop();
				if (m_thread) {
					m_thread->join();
					m_thread.reset();
				}
				delete m_pWebEm;
				m_pWebEm = NULL;
			}
			catch (...)
			{

			}
		}