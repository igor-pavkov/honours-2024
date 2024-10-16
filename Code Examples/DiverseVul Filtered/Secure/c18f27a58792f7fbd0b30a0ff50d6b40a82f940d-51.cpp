Node *TY_(FindHEAD)( TidyDocImpl* doc )
{
    Node *node = TY_(FindHTML)( doc );

    if ( node )
    {
        for ( node = node->content;
              node && !nodeIsHEAD(node); 
              node = node->next )
            /**/;
    }

    return node;
}