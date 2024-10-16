		void CWebServer::GetAppCache(WebEmSession & session, const request& req, reply & rep)
		{
			std::string response = "";
			if (g_bDontCacheWWW)
			{
				return;
			}
			//Return the appcache file (dynamically generated)
			std::string sLine;
			std::string filename = szWWWFolder + "/html5.appcache";


			std::string sWebTheme = "default";
			m_sql.GetPreferencesVar("WebTheme", sWebTheme);

			//Get Dynamic Theme Files
			std::map<std::string, int> _ThemeFiles;
			GetDirFilesRecursive(szWWWFolder + "/styles/" + sWebTheme + "/", _ThemeFiles);

			//Get Dynamic Floorplan Images from database
			std::map<std::string, int> _FloorplanFiles;
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID FROM Floorplans ORDER BY [Order]");
			if (!result.empty())
			{
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;
					std::string ImageURL = "images/floorplans/plan?idx=" + sd[0];
					_FloorplanFiles[ImageURL] = 1;
				}
			}

			std::ifstream is(filename.c_str());
			if (is)
			{
				while (!is.eof())
				{
					getline(is, sLine);
					if (!sLine.empty())
					{
						if (sLine.find("#BuildHash") != std::string::npos)
						{
							stdreplace(sLine, "#BuildHash", szAppHash);
						}
						else if (sLine.find("#ThemeFiles") != std::string::npos)
						{
							response += "#Theme=" + sWebTheme + '\n';
							//Add all theme files
							for (const auto & itt : _ThemeFiles)
							{
								std::string tfname = itt.first.substr(szWWWFolder.size() + 1);
								stdreplace(tfname, "styles/" + sWebTheme, "acttheme");
								response += tfname + '\n';
							}
							continue;
						}
						else if (sLine.find("#Floorplans") != std::string::npos)
						{
							//Add all floorplans
							for (const auto & itt : _FloorplanFiles)
							{
								std::string tfname = itt.first;
								response += tfname + '\n';
							}
							continue;
						}
						else if (sLine.find("#SwitchIcons") != std::string::npos)
						{
							//Add database switch icons
							for (const auto & itt : m_custom_light_icons)
							{
								if (itt.idx >= 100)
								{
									std::string IconFile16 = itt.RootFile + ".png";
									std::string IconFile48On = itt.RootFile + "48_On.png";
									std::string IconFile48Off = itt.RootFile + "48_Off.png";

									response += "images/" + CURLEncode::URLEncode(IconFile16) + '\n';
									response += "images/" + CURLEncode::URLEncode(IconFile48On) + '\n';
									response += "images/" + CURLEncode::URLEncode(IconFile48Off) + '\n';
								}
							}
						}
					}
					response += sLine + '\n';
				}
			}
			reply::set_content(&rep, response);
		}