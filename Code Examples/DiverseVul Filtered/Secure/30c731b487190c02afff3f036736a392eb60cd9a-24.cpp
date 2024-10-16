bool HtmlOutputDev::newHtmlOutlineLevel(FILE *output, const std::vector<OutlineItem*> *outlines, int level)
{
	bool atLeastOne = false;

	if (level == 1)
	{
		fputs("<a name=\"outline\"></a>", output);
		fputs("<h1>Document Outline</h1>\n", output);
	}
	fputs("<ul>\n",output);

	for (std::size_t i = 0; i < outlines->size(); i++)
	{
		OutlineItem *item = (*outlines)[i];
		GooString *titleStr = HtmlFont::HtmlFilter(item->getTitle(),
							   item->getTitleLength());

		GooString *linkName = nullptr;;
        int page = getOutlinePageNum(item);
        if (page > 0)
        {
				/*		complex		simple
				frames		file-4.html	files.html#4
				noframes	file.html#4	file.html#4
				*/
				linkName = new GooString(gbasename(Docname->c_str()));
				GooString *str=GooString::fromInt(page);
				if (noframes) {
					linkName->append(".html#");
					linkName->append(str);
				} else {
					if( complexMode ) {
						linkName->append("-");
						linkName->append(str);
						linkName->append(".html");
					} else {
						linkName->append("s.html#");
						linkName->append(str);
					}
				}
				delete str;
		}

		fputs("<li>",output);
		if (linkName)
			fprintf(output,"<a href=\"%s\">", linkName->c_str());
		fputs(titleStr->c_str(),output);
		if (linkName) {
			fputs("</a>",output);
			delete linkName;
		}
		delete titleStr;
		atLeastOne = true;

		item->open();
		if (item->hasKids() && item->getKids())
		{
			fputs("\n",output);
			newHtmlOutlineLevel(output, item->getKids(), level+1);
		}
		item->close();
		fputs("</li>\n",output);
	}
	fputs("</ul>\n",output);

	return atLeastOne;
}