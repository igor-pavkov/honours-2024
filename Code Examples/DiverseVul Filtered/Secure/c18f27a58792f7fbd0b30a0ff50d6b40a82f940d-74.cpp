void TY_(RemoveAttribute)( TidyDocImpl* doc, Node *node, AttVal *attr )
{
    TY_(DetachAttribute)( node, attr );
    TY_(FreeAttribute)( doc, attr );
}