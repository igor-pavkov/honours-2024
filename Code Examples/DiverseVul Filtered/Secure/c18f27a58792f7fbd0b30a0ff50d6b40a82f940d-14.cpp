void TY_(FreeAttribute)( TidyDocImpl* doc, AttVal *av )
{
    TY_(FreeNode)( doc, av->asp );
    TY_(FreeNode)( doc, av->php );
    TidyDocFree( doc, av->attribute );
    TidyDocFree( doc, av->value );
    TidyDocFree( doc, av );
}