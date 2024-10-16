void TY_(FreeAttrs)( TidyDocImpl* doc, Node *node )
{
    while ( node->attributes )
    {
        AttVal *av = node->attributes;

        if ( av->attribute )
        {
            if ( (attrIsID(av) || attrIsNAME(av)) &&
                 TY_(IsAnchorElement)(doc, node) )
            {
                TY_(RemoveAnchorByNode)( doc, av->value, node );
            }
        }

        node->attributes = av->next;
        TY_(FreeAttribute)( doc, av );
    }
}