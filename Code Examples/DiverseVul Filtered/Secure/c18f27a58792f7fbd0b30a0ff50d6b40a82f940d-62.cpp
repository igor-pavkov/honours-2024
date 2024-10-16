AttVal* TY_(NewAttributeEx)( TidyDocImpl* doc, ctmbstr name, ctmbstr value,
                             int delim )
{
    AttVal *av = TY_(NewAttribute)(doc);
    av->attribute = TY_(tmbstrdup)(doc->allocator, name);
    av->value = TY_(tmbstrdup)(doc->allocator, value);
    av->delim = delim;
    av->dict = TY_(FindAttribute)( doc, av );
    return av;
}