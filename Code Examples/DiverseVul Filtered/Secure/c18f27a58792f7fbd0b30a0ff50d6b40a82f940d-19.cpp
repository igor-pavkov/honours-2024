Node *TY_(FindBody)( TidyDocImpl* doc )
{
    Node *node = ( doc ? doc->root.content : NULL );

    while ( node && !nodeIsHTML(node) )
        node = node->next;

    if (node == NULL)
        return NULL;

    node = node->content;
    while ( node && !nodeIsBODY(node) && !nodeIsFRAMESET(node) )
        node = node->next;

    if ( node && nodeIsFRAMESET(node) )
    {
        node = node->content;
        while ( node && !nodeIsNOFRAMES(node) )
            node = node->next;

        if ( node )
        {
            node = node->content;
            while ( node && !nodeIsBODY(node) )
                node = node->next;
        }
    }

    return node;
}