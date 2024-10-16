Node *TY_(CloneNode)( TidyDocImpl* doc, Node *element )
{
    Lexer* lexer = doc->lexer;
    Node *node = TY_(NewNode)( lexer->allocator, lexer );

    node->start = lexer->lexsize;
    node->end   = lexer->lexsize;

    if ( element )
    {
        node->parent     = element->parent;
        node->type       = element->type;
        node->closed     = element->closed;
        node->implicit   = element->implicit;
        node->tag        = element->tag;
        node->element    = TY_(tmbstrdup)( doc->allocator, element->element );
        node->attributes = TY_(DupAttrs)( doc, element->attributes );
    }
    return node;
}