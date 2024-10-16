int TY_(HTMLVersion)(TidyDocImpl* doc)
{
    uint i;
    uint j = 0;
    uint score = 0;
    uint vers = doc->lexer->versions;
    uint dtver = doc->lexer->doctype;
    TidyDoctypeModes dtmode = (TidyDoctypeModes)cfg(doc, TidyDoctypeMode);
    Bool xhtml = (cfgBool(doc, TidyXmlOut) || doc->lexer->isvoyager) &&
                 !cfgBool(doc, TidyHtmlOut);
    Bool html4 = dtmode == TidyDoctypeStrict || dtmode == TidyDoctypeLoose || VERS_FROM40 & dtver;

    if (xhtml && dtver == VERS_UNKNOWN) return XH50;
    if (dtver == VERS_UNKNOWN) return HT50;
    /* Issue #167 - if NOT XHTML, and doctype is default VERS_HTML5, then return HT50 */
    if (!xhtml && (dtver == VERS_HTML5)) return HT50;

    for (i = 0; W3C_Doctypes[i].name; ++i)
    {
        if ((xhtml && !(VERS_XHTML & W3C_Doctypes[i].vers)) ||
            (html4 && !(VERS_FROM40 & W3C_Doctypes[i].vers)))
            continue;

        if (vers & W3C_Doctypes[i].vers &&
            (W3C_Doctypes[i].score < score || !score))
        {
            score = W3C_Doctypes[i].score;
            j = i;
        }
    }

    if (score)
        return W3C_Doctypes[j].vers;

    return VERS_UNKNOWN;
}