static Node* NewDocTypeNode( TidyDocImpl* doc )
{
    Node* doctype = NULL;
    Node* html = TY_(FindHTML)( doc );

    if ( !html )
        return NULL;

    doctype = TY_(NewNode)( doc->allocator, NULL );
    doctype->type = DocTypeTag;
    TY_(InsertNodeBeforeElement)(html, doctype);
    return doctype;
}