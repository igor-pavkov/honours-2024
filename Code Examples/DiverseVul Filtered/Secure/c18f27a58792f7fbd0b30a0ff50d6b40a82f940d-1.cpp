void TY_(FreeNode)( TidyDocImpl* doc, Node *node )
{
#if !defined(NDEBUG) && defined(_MSC_VER) && defined(DEBUG_ALLOCATION)
    if (node) SPRTF("Free node %p\n", node );
#endif
    /* this is no good ;=((
    if (node && doc && doc->lexer) {
        if (node == doc->lexer->token) {
            doc->lexer->token = NULL; // TY_(NewNode)( doc->lexer->allocator, doc->lexer );
        }
    }
      ----------------- */
    while ( node )
    {
        Node* next = node->next;

        TY_(FreeAttrs)( doc, node );
        TY_(FreeNode)( doc, node->content );
        TidyDocFree( doc, node->element );
#ifdef TIDY_STORE_ORIGINAL_TEXT
        if (node->otext)
            TidyDocFree(doc, node->otext);
#endif
        if (RootNode != node->type)
            TidyDocFree( doc, node );
        else
            node->content = NULL;

        node = next;
    }
}