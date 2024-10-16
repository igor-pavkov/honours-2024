Node *TY_(FindTITLE)(TidyDocImpl* doc)
{
    Node *node = TY_(FindHEAD)(doc);

    if (node)
        for (node = node->content;
             node && !nodeIsTITLE(node);
             node = node->next) {}

    return node;
}