Bool TY_(FixDocType)( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    Node* doctype = TY_(FindDocType)( doc );
    uint dtmode = cfg( doc, TidyDoctypeMode );
    uint guessed = VERS_UNKNOWN;
    Bool hadSI = no;

    /* Issue #167 - found doctype, and doctype is default VERS_HTML5, set VERS_HTML5 and return yes */
    if (doctype && (dtmode == TidyDoctypeAuto) &&
        (lexer->doctype == VERS_HTML5) )
    {
        lexer->versionEmitted = lexer->doctype;
        return yes;
    }
    if (dtmode == TidyDoctypeAuto &&
        lexer->versions & lexer->doctype &&
        !(VERS_XHTML & lexer->doctype && !lexer->isvoyager)
        && TY_(FindDocType)(doc))
    {
        lexer->versionEmitted = lexer->doctype;
        return yes;
    }

    if (dtmode == TidyDoctypeOmit)
    {
        if (doctype)
            TY_(DiscardElement)( doc, doctype );
        lexer->versionEmitted = TY_(ApparentVersion)( doc );
        return yes;
    }

    if (cfgBool(doc, TidyXmlOut))
        return yes;

    if (doctype)
        hadSI = TY_(GetAttrByName)(doctype, "SYSTEM") != NULL;

    if ((dtmode == TidyDoctypeStrict ||
         dtmode == TidyDoctypeLoose) && doctype)
    {
        TY_(DiscardElement)(doc, doctype);
        doctype = NULL;
    }

    switch (dtmode)
    {
    case TidyDoctypeHtml5:
        guessed = HT50;
        break;
    case TidyDoctypeStrict:
        guessed = H41S;
        break;
    case TidyDoctypeLoose:
        guessed = H41T;
        break;
    case TidyDoctypeAuto:
        guessed = TY_(HTMLVersion)(doc);
        break;
    }

    lexer->versionEmitted = guessed;
    if (guessed == VERS_UNKNOWN)
        return no;

    if (doctype)
    {
        doctype->element = TY_(tmbstrtolower)(doctype->element);
    }
    else
    {
        doctype = NewDocTypeNode(doc);
        doctype->element = TY_(tmbstrdup)(doc->allocator, "html");
    }

    TY_(RepairAttrValue)(doc, doctype, "PUBLIC", GetFPIFromVers(guessed));

    if (hadSI)
        TY_(RepairAttrValue)(doc, doctype, "SYSTEM", GetSIFromVers(guessed));

    return yes;
}