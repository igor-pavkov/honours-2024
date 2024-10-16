Lexer* TY_(NewLexer)( TidyDocImpl* doc )
{
    Lexer* lexer = (Lexer*) TidyDocAlloc( doc, sizeof(Lexer) );

    if ( lexer != NULL )
    {
        TidyClearMemory( lexer, sizeof(Lexer) );

        lexer->allocator = doc->allocator;
        lexer->lines = 1;
        lexer->columns = 1;
        lexer->state = LEX_CONTENT;

        lexer->versions = (VERS_ALL|VERS_PROPRIETARY);
        lexer->doctype = VERS_UNKNOWN;
        lexer->root = &doc->root;
    }
    return lexer;
}