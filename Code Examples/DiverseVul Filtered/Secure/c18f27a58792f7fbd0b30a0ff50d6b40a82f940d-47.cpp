static uint FindGivenVersion( TidyDocImpl* doc, Node* doctype )
{
    AttVal * fpi = TY_(GetAttrByName)(doctype, "PUBLIC");
    uint vers;

    if (!fpi || !fpi->value) 
    {
        if (doctype->element && (TY_(tmbstrcmp)(doctype->element,"html") == 0))
        {
            return VERS_HTML5;  /* TODO: do we need to check MORE? */
        }
        /* TODO: Consider warning, error message */
        return VERS_UNKNOWN;
    }
    vers = GetVersFromFPI(fpi->value);

    if (VERS_XHTML & vers)
    {
        TY_(SetOptionBool)(doc, TidyXmlOut, yes);
        TY_(SetOptionBool)(doc, TidyXhtmlOut, yes);
        doc->lexer->isvoyager = yes;
    }

    /* todo: add a warning if case does not match? */
    TidyDocFree(doc, fpi->value);
    fpi->value = TY_(tmbstrdup)(doc->allocator, GetFPIFromVers(vers));

    return vers;
}