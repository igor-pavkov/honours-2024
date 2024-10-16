		void CWebServer::RType_CustomLightIcons(WebEmSession & session, const request& req, Json::Value &root)
		{
			int ii = 0;

			std::vector<_tCustomIcon> temp_custom_light_icons = m_custom_light_icons;
			//Sort by name
			std::sort(temp_custom_light_icons.begin(), temp_custom_light_icons.end(), compareIconsByName);

			for (const auto & itt : temp_custom_light_icons)
			{
				root["result"][ii]["idx"] = itt.idx;
				root["result"][ii]["imageSrc"] = itt.RootFile;
				root["result"][ii]["text"] = itt.Title;
				root["result"][ii]["description"] = itt.Description;
				ii++;
			}
			root["status"] = "OK";
		}