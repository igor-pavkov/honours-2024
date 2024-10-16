Node *TY_(NewNode)(TidyAllocator* allocator, Lexer *lexer)
{
    Node* node = (Node*) TidyAlloc( allocator, sizeof(Node) );
    TidyClearMemory( node, sizeof(Node) );
    if ( lexer )
    {
        node->line = lexer->lines;
        node->column = lexer->columns;
    }
    node->type = TextNode;
#if !defined(NDEBUG) && defined(_MSC_VER) && defined(DEBUG_ALLOCATION)
    SPRTF("Allocated node %p\n", node );
#endif
    return node;
}