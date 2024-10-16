static int ParseServerInstruction( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    uint c;
    int delim = '"';
    Bool isrule = no;

    c = TY_(ReadChar)(doc->docIn);
    TY_(AddCharToLexer)(lexer, c);

    /* check for ASP, PHP or Tango */
    if (c == '%' || c == '?' || c == '@')
        isrule = yes;

    for (;;)
    {
        c = TY_(ReadChar)(doc->docIn);

        if (c == EndOfStream)
            break;

        if (c == '>')
        {
            if (isrule)
                TY_(AddCharToLexer)(lexer, c);
            else
                TY_(UngetChar)(c, doc->docIn);

            break;
        }

        /* if not recognized as ASP, PHP or Tango */
        /* then also finish value on whitespace */
        if (!isrule)
        {
            if (TY_(IsWhite)(c))
                break;
        }

        TY_(AddCharToLexer)(lexer, c);

        if (c == '"')
        {
            do
            {
                c = TY_(ReadChar)(doc->docIn);
                if (c == EndOfStream) /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
                    TY_(UngetChar)(c, doc->docIn);
                    return 0;
                }
                if (c == '>') /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(UngetChar)(c, doc->docIn);
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
                    return 0;
                }
                TY_(AddCharToLexer)(lexer, c);
            }
            while (c != '"');
            delim = '\'';
            continue;
        }

        if (c == '\'')
        {
            do
            {
                c = TY_(ReadChar)(doc->docIn);
                if (c == EndOfStream) /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
                    TY_(UngetChar)(c, doc->docIn);
                    return 0;
                }
                if (c == '>') /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(UngetChar)(c, doc->docIn);
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
                    return 0;
                }
                TY_(AddCharToLexer)(lexer, c);
            }
            while (c != '\'');
        }
    }

    return delim;
}