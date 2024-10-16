static Node* NewToken(TidyDocImpl* doc, NodeType type)
{
    Lexer* lexer = doc->lexer;
    Node* node = TY_(NewNode)(lexer->allocator, lexer);
    node->type = type;
    node->start = lexer->txtstart;
    node->end = lexer->txtend;
#ifdef TIDY_STORE_ORIGINAL_TEXT
    StoreOriginalTextInToken(doc, node, 0);
#endif
    return node;
}