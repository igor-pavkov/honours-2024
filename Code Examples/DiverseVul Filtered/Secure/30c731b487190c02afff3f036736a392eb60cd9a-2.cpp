void HtmlOutputDev::newXmlOutlineLevel(FILE *output, const std::vector<OutlineItem*> *outlines)
{
    fputs("<outline>\n", output);

    for (std::size_t i = 0; i < outlines->size(); i++)
    {
        OutlineItem *item     = (*outlines)[i];
        GooString   *titleStr = HtmlFont::HtmlFilter(item->getTitle(),
                                                     item->getTitleLength());
        int page = getOutlinePageNum(item);
        if (page > 0)
        {
            fprintf(output, "<item page=\"%d\">%s</item>\n",
                    page, titleStr->c_str());
        }
        else
        {
            fprintf(output, "<item>%s</item>\n", titleStr->c_str());
        }
        delete titleStr;

        item->open();
        if (item->hasKids() && item->getKids())
        {
            newXmlOutlineLevel(output, item->getKids());
        }
        item->close();
    }    

    fputs("</outline>\n", output);
}