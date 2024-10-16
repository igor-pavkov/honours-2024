GooString* HtmlMetaVar::toString()	
{
    GooString *result = new GooString("<meta name=\"");
    result->append(name);
    result->append("\" content=\"");
    result->append(content);
    result->append("\"/>");
    return result;
}