		void CWebServer::GetJSonPage(WebEmSession & session, const request& req, reply & rep)
		{
			Json::Value root;
			root["status"] = "ERR";

			std::string rtype = request::findValue(&req, "type");
			if (rtype == "command")
			{
				std::string cparam = request::findValue(&req, "param");
				if (cparam.empty())
				{
					cparam = request::findValue(&req, "dparam");
					if (cparam.empty())
					{
						goto exitjson;
					}
				}
				if (cparam == "dologout")
				{
					session.forcelogin = true;
					root["status"] = "OK";
					root["title"] = "Logout";
					goto exitjson;

				}
				_log.Debug(DEBUG_WEBSERVER, "WEBS GetJSon :%s :%s ", cparam.c_str(), req.uri.c_str());
				HandleCommand(cparam, session, req, root);
			} //(rtype=="command")
			else {
				HandleRType(rtype, session, req, root);
			}
		exitjson:
			std::string jcallback = request::findValue(&req, "jsoncallback");
			if (jcallback.size() == 0) {
				reply::set_content(&rep, root.toStyledString());
				return;
			}
			reply::set_content(&rep, "var data=" + root.toStyledString() + '\n' + jcallback + "(data);");
		}