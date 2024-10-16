		void CWebServer::RegisterCommandCode(const char* idname, webserver_response_function ResponseFunction, bool bypassAuthentication)
		{
			m_webcommands.insert(std::pair<std::string, webserver_response_function >(std::string(idname), ResponseFunction));
			if (bypassAuthentication)
			{
				m_pWebEm->RegisterWhitelistURLString(idname);
			}
		}