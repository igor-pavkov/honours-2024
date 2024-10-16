Node* TY_(NewLiteralTextNode)( Lexer *lexer, ctmbstr txt )
{
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    node->start = lexer->lexsize;
    AddStringToLexer( lexer, txt );
    node->end = lexer->lexsize;
    return node;
}