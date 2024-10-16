Node *TY_(FindXmlDecl)(TidyDocImpl* doc)
{
    Node *node;
    for ( node = (doc ? doc->root.content : NULL);
          node && !(node->type == XmlDecl);
          node = node->next )
        /**/;

    return node;
}