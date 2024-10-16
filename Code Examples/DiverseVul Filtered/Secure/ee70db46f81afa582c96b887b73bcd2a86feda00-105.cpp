		void CWebServer::UploadFloorplanImage(WebEmSession & session, const request& req, std::string & redirect_uri)
		{
			redirect_uri = "/index.html";
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string planname = request::findValue(&req, "planname");
			std::string scalefactor = request::findValue(&req, "scalefactor");
			std::string imagefile = request::findValue(&req, "imagefile");

			std::vector<std::vector<std::string> > result;
			m_sql.safe_query("INSERT INTO Floorplans ([Name],[ScaleFactor]) VALUES('%s','%s')", planname.c_str(),scalefactor.c_str());
			result = m_sql.safe_query("SELECT MAX(ID) FROM Floorplans");
			if (!result.empty())
			{
				if (!m_sql.safe_UpdateBlobInTableWithID("Floorplans", "Image", result[0][0], imagefile))
					_log.Log(LOG_ERROR, "SQL: Problem inserting floorplan image into database! ");
			}
		}