		void CWebServer::DisplayTimerTypesCombo(std::string & content_part)
		{
			char szTmp[200];
			for (int ii = 0; ii < TTYPE_END; ii++)
			{
				sprintf(szTmp, "<option data-i18n=\"%s\" value=\"%d\">%s</option>\n", Timer_Type_Desc(ii), ii, Timer_Type_Desc(ii));
				content_part += szTmp;
			}
		}