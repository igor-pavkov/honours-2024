static Node* TagToken( TidyDocImpl* doc, NodeType type )
{
    Lexer* lexer = doc->lexer;
    Node* node = TY_(NewNode)( lexer->allocator, lexer );
    node->type = type;
    node->element = TY_(tmbstrndup)( doc->allocator,
                                     lexer->lexbuf + lexer->txtstart,
                                     lexer->txtend - lexer->txtstart );
    node->start = lexer->txtstart;
    node->end = lexer->txtstart;

    if ( type == StartTag || type == StartEndTag || type == EndTag )
        TY_(FindTag)(doc, node);

    return node;
}