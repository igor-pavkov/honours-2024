char FoldCase( TidyDocImpl* doc, tmbchar c, Bool tocaps )
{
    if ( !cfgBool(doc, TidyXmlTags) )
    {
        if ( tocaps )
        {
            c = (tmbchar) ToUpper(c);
        }
        else /* force to lower case */
        {
            c = (tmbchar) ToLower(c);
        }
    }
    return c;
}