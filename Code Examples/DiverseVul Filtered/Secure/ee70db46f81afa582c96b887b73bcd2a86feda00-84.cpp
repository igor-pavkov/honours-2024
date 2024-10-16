		void CWebServer::DisplayMeterTypesCombo(std::string & content_part)
		{
			char szTmp[200];
			for (int ii = 0; ii < MTYPE_END; ii++)
			{
				sprintf(szTmp, "<option value=\"%d\">%s</option>\n", ii, Meter_Type_Desc((_eMeterType)ii));
				content_part += szTmp;
			}
		}