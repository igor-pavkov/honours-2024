uint TY_(ApparentVersion)( TidyDocImpl* doc )
{
    if ((doc->lexer->doctype == XH11 ||
         doc->lexer->doctype == XB10) &&
        (doc->lexer->versions & doc->lexer->doctype))
        return doc->lexer->doctype;
    else
        return TY_(HTMLVersion)(doc);
}