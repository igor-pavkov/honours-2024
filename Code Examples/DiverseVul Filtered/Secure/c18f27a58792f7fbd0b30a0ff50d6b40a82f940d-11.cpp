Node *TY_(FindHTML)( TidyDocImpl* doc )
{
    Node *node;
    for ( node = (doc ? doc->root.content : NULL);
          node && !nodeIsHTML(node); 
          node = node->next )
        /**/;

    return node;
}