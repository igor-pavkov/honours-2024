void TY_(InsertAttributeAtStart)( Node *node, AttVal *av )
{
    av->next = node->attributes;
    node->attributes = av;
}