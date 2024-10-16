		void CWebServer::DisplaySwitchTypesCombo(std::string & content_part)
		{
			char szTmp[200];

			std::map<std::string, int> _switchtypes;

			for (int ii = 0; ii < STYPE_END; ii++)
			{
				_switchtypes[Switch_Type_Desc((_eSwitchType)ii)] = ii;
			}
			//return a sorted list
			for (const auto & itt : _switchtypes)
			{
				sprintf(szTmp, "<option value=\"%d\">%s</option>\n", itt.second, itt.first.c_str());
				content_part += szTmp;
			}
		}