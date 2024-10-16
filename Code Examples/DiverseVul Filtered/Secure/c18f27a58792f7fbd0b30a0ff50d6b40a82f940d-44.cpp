AttVal *TY_(NewAttribute)( TidyDocImpl* doc )
{
    AttVal *av = (AttVal*) TidyDocAlloc( doc, sizeof(AttVal) );
    TidyClearMemory( av, sizeof(AttVal) );
    return av;
}