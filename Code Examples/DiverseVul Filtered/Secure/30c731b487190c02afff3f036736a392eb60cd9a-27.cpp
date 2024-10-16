bool HtmlOutputDev::dumpDocOutline(PDFDoc* doc)
{ 
	FILE * output = nullptr;
	bool bClose = false;

	if (!ok)
                return false;
  
	Outline *outline = doc->getOutline();
	if (!outline)
		return false;

	const std::vector<OutlineItem*> *outlines = outline->getItems();
	if (!outlines)
		return false;
  
	if (!complexMode || xml)
  	{
		output = page;
  	}
  	else if (complexMode && !xml)
	{
		if (noframes)
		{
			output = page; 
			fputs("<hr/>\n", output);
		}
		else
		{
			GooString *str = Docname->copy();
			str->append("-outline.html");
			output = fopen(str->c_str(), "w");
			delete str;
			if (output == nullptr)
				return false;
			bClose = true;

			GooString *htmlEncoding =
				HtmlOutputDev::mapEncodingToHtml(globalParams->getTextEncodingName());

			fprintf(output, "<html xmlns=\"http://www.w3.org/1999/xhtml\" " \
                                "lang=\"\" xml:lang=\"\">\n"            \
                                "<head>\n"                              \
                                "<title>Document Outline</title>\n"     \
                                "<meta http-equiv=\"Content-Type\" content=\"text/html; " \
                                "charset=%s\"/>\n"                      \
                                "</head>\n<body>\n", htmlEncoding->c_str());
			delete htmlEncoding;
		}
	}
 
	if (!xml)
	{
		bool done = newHtmlOutlineLevel(output, outlines);
		if (done && !complexMode)
			fputs("<hr/>\n", output);
	
		if (bClose)
		{
			fputs("</body>\n</html>\n", output);
			fclose(output);
		}
	}
	else
		newXmlOutlineLevel(output, outlines);

	return true;
}