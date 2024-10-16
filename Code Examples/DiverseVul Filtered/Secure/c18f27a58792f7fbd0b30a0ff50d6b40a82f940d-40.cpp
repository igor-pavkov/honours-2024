static AttVal* ParseAttrs( TidyDocImpl* doc, Bool *isempty )
{
    Lexer* lexer = doc->lexer;
    AttVal *av, *list;
    tmbstr value;
    int delim;
    Node *asp, *php;

    list = NULL;

    while ( !EndOfInput(doc) )
    {
        tmbstr attribute = ParseAttribute( doc, isempty, &asp, &php );

        if (attribute == NULL)
        {
            /* check if attributes are created by ASP markup */
            if (asp)
            {
                av = TY_(NewAttribute)(doc);
                av->asp = asp;
                AddAttrToList( &list, av ); 
                continue;
            }

            /* check if attributes are created by PHP markup */
            if (php)
            {
                av = TY_(NewAttribute)(doc);
                av->php = php;
                AddAttrToList( &list, av ); 
                continue;
            }

            break;
        }

        value = ParseValue( doc, attribute, no, isempty, &delim );

        if (attribute && (IsValidAttrName(attribute) ||
            (cfgBool(doc, TidyXmlTags) && IsValidXMLAttrName(attribute))))
        {
            av = TY_(NewAttribute)(doc);
            av->delim = delim;
            av->attribute = attribute;
            av->value = value;
            av->dict = TY_(FindAttribute)( doc, av );
            AddAttrToList( &list, av ); 
        }
        else
        {
            av = TY_(NewAttribute)(doc);
            av->attribute = attribute;
            av->value = value;

            if (LastChar(attribute) == '"')
                TY_(ReportAttrError)( doc, lexer->token, av, MISSING_QUOTEMARK);
            else if (value == NULL)
                TY_(ReportAttrError)(doc, lexer->token, av, MISSING_ATTR_VALUE);
            else
                TY_(ReportAttrError)(doc, lexer->token, av, INVALID_ATTRIBUTE);

            TY_(FreeAttribute)( doc, av );
        }
    }

    return list;
}