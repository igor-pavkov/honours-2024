Node *TY_(NewLineNode)( Lexer *lexer )
{
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    node->start = lexer->lexsize;
    TY_(AddCharToLexer)( lexer, (uint)'\n' );
    node->end = lexer->lexsize;
    return node;
}