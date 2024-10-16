static Node *ParseAsp( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    uint c;
    Node *asp = NULL;

    lexer->txtstart = lexer->lexsize;

    for (;;)
    {
        if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
            break;

        TY_(AddCharToLexer)(lexer, c);


        if (c != '%')
            continue;

        if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
            break;

        TY_(AddCharToLexer)(lexer, c);

        if (c == '>')
        {
            lexer->lexsize -= 2;
            break;
        }
    }

    lexer->txtend = lexer->lexsize;
    if (lexer->txtend > lexer->txtstart)
        asp = AspToken(doc);

    lexer->txtstart = lexer->txtend;
    return asp;
}   