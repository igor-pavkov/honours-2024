static tmbchar ParseTagName( TidyDocImpl* doc )
{
    Lexer *lexer = doc->lexer;
    uint c = lexer->lexbuf[ lexer->txtstart ];
    Bool xml = cfgBool(doc, TidyXmlTags);

    /* fold case of first character in buffer */
    if (!xml && TY_(IsUpper)(c))
        lexer->lexbuf[lexer->txtstart] = (tmbchar) TY_(ToLower)(c);

    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream)
    {
        if ((!xml && !TY_(IsNamechar)(c)) ||
            (xml && !TY_(IsXMLNamechar)(c)))
            break;

        /* fold case of subsequent characters */
        if (!xml && TY_(IsUpper)(c))
             c = TY_(ToLower)(c);

        TY_(AddCharToLexer)(lexer, c);
    }

    lexer->txtend = lexer->lexsize;
    return (tmbchar) c;
}