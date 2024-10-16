		void CWebServer::Cmd_ExcecuteScript(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string scriptname = request::findValue(&req, "scriptname");
			if (scriptname.empty())
				return;
			if (scriptname.find("..") != std::string::npos)
				return;
#ifdef WIN32
			scriptname = szUserDataFolder + "scripts\\" + scriptname;
#else
			scriptname = szUserDataFolder + "scripts/" + scriptname;
#endif
			if (!file_exist(scriptname.c_str()))
				return;
			std::string script_params = request::findValue(&req, "scriptparams");
			std::string strparm = szUserDataFolder;
			if (!script_params.empty())
			{
				if (strparm.size() > 0)
					strparm += " " + script_params;
				else
					strparm = script_params;
			}
			std::string sdirect = request::findValue(&req, "direct");
			if (sdirect == "true")
			{
				_log.Log(LOG_STATUS, "Executing script: %s", scriptname.c_str());
#ifdef WIN32
				ShellExecute(NULL, "open", scriptname.c_str(), strparm.c_str(), NULL, SW_SHOWNORMAL);
#else
				std::string lscript = scriptname + " " + strparm;
				int ret = system(lscript.c_str());
				if (ret != 0)
				{
					_log.Log(LOG_ERROR, "Error executing script command (%s). returned: %d", lscript.c_str(), ret);
					return;
			}
#endif
		}
			else
			{
				//add script to background worker
				m_sql.AddTaskItem(_tTaskItem::ExecuteScript(0.2f, scriptname, strparm));
			}
			root["title"] = "ExecuteScript";
			root["status"] = "OK";
		}