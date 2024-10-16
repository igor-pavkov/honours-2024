		void CWebServer::DisplayLanguageCombo(std::string & content_part)
		{
			//return a sorted list
			std::map<std::string, std::string> _ltypes;
			char szTmp[200];
			int ii = 0;
			while (guiLanguage[ii].szShort != NULL)
			{
				_ltypes[guiLanguage[ii].szLong] = guiLanguage[ii].szShort;
				ii++;
			}
			for (const auto & itt : _ltypes)
			{
				sprintf(szTmp, "<option value=\"%s\">%s</option>\n", itt.second.c_str(), itt.first.c_str());
				content_part += szTmp;
			}
		}