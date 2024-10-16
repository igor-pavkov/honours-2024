		void CWebServer::GetDatabaseBackup(WebEmSession & session, const request& req, reply & rep)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
#ifdef WIN32
			std::string OutputFileName = szUserDataFolder + "backup.db";
#else
			std::string OutputFileName = "/tmp/backup.db";
#endif
			if (m_sql.BackupDatabase(OutputFileName))
			{
				std::string szAttachmentName = "domoticz.db";
				std::string szVar;
				if (m_sql.GetPreferencesVar("Title", szVar))
				{
					stdreplace(szVar, " ", "_");
					stdreplace(szVar, "/", "_");
					stdreplace(szVar, "\\", "_");
					if (!szVar.empty()) {
						szAttachmentName = szVar + ".db";
					}
				}
				reply::set_content_from_file(&rep, OutputFileName, szAttachmentName, true);
			}
		}