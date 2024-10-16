Bool TY_(SetXHTMLDocType)( TidyDocImpl* doc )
{
    Lexer *lexer = doc->lexer;
    Node *doctype = TY_(FindDocType)( doc );
    TidyDoctypeModes dtmode = (TidyDoctypeModes)cfg(doc, TidyDoctypeMode);
    ctmbstr pub = "PUBLIC";
    ctmbstr sys = "SYSTEM";

    lexer->versionEmitted = TY_(ApparentVersion)( doc );

    if (dtmode == TidyDoctypeOmit)
    {
        if (doctype)
            TY_(DiscardElement)(doc, doctype);
        return yes;
    }

    if (dtmode == TidyDoctypeUser && !cfgStr(doc, TidyDoctype))
        return no;

    if (!doctype)
    {
        doctype = NewDocTypeNode(doc);
        doctype->element = TY_(tmbstrdup)(doc->allocator, "html");
    }
    else
    {
        doctype->element = TY_(tmbstrtolower)(doctype->element);
    }

    switch(dtmode)
    {
    case TidyDoctypeHtml5:
        /* HTML5 */
        TY_(RepairAttrValue)(doc, doctype, pub, NULL);
        TY_(RepairAttrValue)(doc, doctype, sys, NULL);
        lexer->versionEmitted = XH50;
        break;
    case TidyDoctypeStrict:
        /* XHTML 1.0 Strict */
        TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10S));
        TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10S));
        lexer->versionEmitted = X10S;
        break;
    case TidyDoctypeLoose:
        /* XHTML 1.0 Transitional */
        TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10T));
        TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10T));
        lexer->versionEmitted = X10T;
        break;
    case TidyDoctypeUser:
        /* user defined document type declaration */
        TY_(RepairAttrValue)(doc, doctype, pub, cfgStr(doc, TidyDoctype));
        TY_(RepairAttrValue)(doc, doctype, sys, "");
        break;
    case TidyDoctypeAuto:
        if (lexer->doctype == VERS_UNKNOWN) {
          lexer->versionEmitted = XH50;
          return yes;
        }
        else if (lexer->versions & XH11 && lexer->doctype == XH11)
        {
            if (!TY_(GetAttrByName)(doctype, sys))
                TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(XH11));
            lexer->versionEmitted = XH11;
            return yes;
        }
        else if (lexer->versions & XH11 && !(lexer->versions & VERS_HTML40))
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(XH11));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(XH11));
            lexer->versionEmitted = XH11;
        }
        else if (lexer->versions & XB10 && lexer->doctype == XB10)
        {
            if (!TY_(GetAttrByName)(doctype, sys))
                TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(XB10));
            lexer->versionEmitted = XB10;
            return yes;
        }
        else if (lexer->versions & VERS_HTML40_STRICT)
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10S));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10S));
            lexer->versionEmitted = X10S;
        }
        else if (lexer->versions & VERS_FRAMESET)
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10F));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10F));
            lexer->versionEmitted = X10F;
        }
        else if (lexer->versions & VERS_LOOSE)
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10T));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10T));
            lexer->versionEmitted = X10T;
        }
        else
        {
            if (doctype)
                TY_(DiscardElement)(doc, doctype);
            return no;
        }
        break;
    case TidyDoctypeOmit:
        assert(0);
        break;
    }

    return no;
}