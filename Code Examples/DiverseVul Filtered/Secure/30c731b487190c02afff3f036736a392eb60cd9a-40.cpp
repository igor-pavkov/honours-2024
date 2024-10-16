HtmlMetaVar::HtmlMetaVar(const char *_name, const char *_content)
{
    name = new GooString(_name);
    content = new GooString(_content);
}