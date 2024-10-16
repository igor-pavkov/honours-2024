static tmbstr  ParseAttribute( TidyDocImpl* doc, Bool *isempty,
                              Node **asp, Node **php)
{
    Lexer* lexer = doc->lexer;
    int start, len = 0;
    tmbstr attr = NULL;
    uint c, lastc;

    *asp = NULL;  /* clear asp pointer */
    *php = NULL;  /* clear php pointer */

 /* skip white space before the attribute */

    for (;;)
    {
        c = TY_(ReadChar)( doc->docIn );


        if (c == '/')
        {
            c = TY_(ReadChar)( doc->docIn );

            if (c == '>')
            {
                *isempty = yes;
                return NULL;
            }

            TY_(UngetChar)(c, doc->docIn);
            c = '/';
            break;
        }

        if (c == '>')
            return NULL;

        if (c =='<')
        {
            c = TY_(ReadChar)(doc->docIn);

            if (c == '%')
            {
                *asp = ParseAsp( doc );
                return NULL;
            }
            else if (c == '?')
            {
                *php = ParsePhp( doc );
                return NULL;
            }

            TY_(UngetChar)(c, doc->docIn);
            TY_(UngetChar)('<', doc->docIn);
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
            return NULL;
        }

        if (c == '=')
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_EQUALSIGN );
            continue;
        }

        if (c == '"' || c == '\'')
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_QUOTEMARK );
            continue;
        }

        if (c == EndOfStream)
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
            TY_(UngetChar)(c, doc->docIn);
            return NULL;
        }


        if (!TY_(IsWhite)(c))
           break;
    }

    start = lexer->lexsize;
    lastc = c;

    for (;;)
    {
     /* but push back '=' for parseValue() */
        if (c == '=' || c == '>')
        {
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (c == '<' || c == EndOfStream)
        {
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (lastc == '-' && (c == '"' || c == '\''))
        {
            lexer->lexsize--;
            --len;
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (TY_(IsWhite)(c))
            break;

        /* what should be done about non-namechar characters? */
        /* currently these are incorporated into the attr name */

        if ( !cfgBool(doc, TidyXmlTags) && TY_(IsUpper)(c) )
            c = TY_(ToLower)(c);

        TY_(AddCharToLexer)( lexer, c );
        lastc = c;
        c = TY_(ReadChar)(doc->docIn);
    }

    /* handle attribute names with multibyte chars */
    len = lexer->lexsize - start;
    attr = (len > 0 ? TY_(tmbstrndup)(doc->allocator,
                                      lexer->lexbuf+start, len) : NULL);
    lexer->lexsize = start;
    return attr;
}