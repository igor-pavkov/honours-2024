		void CWebServer::RegisterRType(const char* idname, webserver_response_function ResponseFunction)
		{
			m_webrtypes.insert(std::pair<std::string, webserver_response_function >(std::string(idname), ResponseFunction));
		}