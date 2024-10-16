Node *TY_(FindDocType)( TidyDocImpl* doc )
{
    Node* node;
    for ( node = (doc ? doc->root.content : NULL);
          node && node->type != DocTypeTag; 
          node = node->next )
        /**/;
    return node;
}