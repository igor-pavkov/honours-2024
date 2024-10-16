		void CWebServer::Cmd_DeleteCustomIcon(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string sidx = request::findValue(&req, "idx");
			if (sidx.empty())
				return;
			int idx = atoi(sidx.c_str());
			root["status"] = "OK";
			root["title"] = "DeleteCustomIcon";

			m_sql.safe_query("DELETE FROM CustomImages WHERE (ID == %d)", idx);

			//Delete icons file from disk
			for (const auto & itt : m_custom_light_icons)
			{
				if (itt.idx == idx + 100)
				{
					std::string IconFile16 = szWWWFolder + "/images/" + itt.RootFile + ".png";
					std::string IconFile48On = szWWWFolder + "/images/" + itt.RootFile + "48_On.png";
					std::string IconFile48Off = szWWWFolder + "/images/" + itt.RootFile + "48_Off.png";
					std::remove(IconFile16.c_str());
					std::remove(IconFile48On.c_str());
					std::remove(IconFile48Off.c_str());
					break;
				}
			}
			ReloadCustomSwitchIcons();
		}