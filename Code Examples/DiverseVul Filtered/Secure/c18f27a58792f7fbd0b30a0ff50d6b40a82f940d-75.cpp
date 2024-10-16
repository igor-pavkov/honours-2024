Node* TY_(TextToken)( Lexer *lexer )
{
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    node->start = lexer->txtstart;
    node->end = lexer->txtend;
    return node;
}