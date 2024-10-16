Node* TY_(InferredTag)(TidyDocImpl* doc, TidyTagId id)
{
    Lexer *lexer = doc->lexer;
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    const Dict* dict = TY_(LookupTagDef)(id);

    assert( dict != NULL );

    node->type = StartTag;
    node->implicit = yes;
    node->element = TY_(tmbstrdup)(doc->allocator, dict->name);
    node->tag = dict;
    node->start = lexer->txtstart;
    node->end = lexer->txtend;

    return node;
}