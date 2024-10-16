		void CWebServer::HandleRType(const std::string &rtype, WebEmSession & session, const request& req, Json::Value &root)
		{
			std::map < std::string, webserver_response_function >::iterator pf = m_webrtypes.find(rtype);
			if (pf != m_webrtypes.end())
			{
				pf->second(session, req, root);
			}
		}