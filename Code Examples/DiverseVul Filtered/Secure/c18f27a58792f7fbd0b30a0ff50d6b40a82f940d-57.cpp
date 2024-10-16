Node* TY_(FindContainer)( Node* node )
{
    for ( node = (node ? node->parent : NULL);
          node && TY_(nodeHasCM)(node, CM_INLINE);
          node = node->parent )
        /**/;

    return node;
}