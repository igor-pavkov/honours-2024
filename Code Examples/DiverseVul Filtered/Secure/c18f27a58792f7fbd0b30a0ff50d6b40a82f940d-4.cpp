Node* TY_(GetToken)( TidyDocImpl* doc, GetTokenMode mode )
{
    Node *node;
    Lexer* lexer = doc->lexer;

    if (lexer->pushed || lexer->itoken)
    {
        /* Deal with previously returned duplicate inline token */
        if (lexer->itoken)
        {
            /* itoken rejected */
            if (lexer->pushed)
            {
                lexer->pushed = no;
                node = lexer->itoken;
                GTDBG(doc,"lex-itoken", node);
                return node;
            }
            /* itoken has been accepted */
            lexer->itoken = NULL;
        }
            
        /* duplicate inlines in preference to pushed text nodes when appropriate */
        lexer->pushed = no;
        if (lexer->token->type != TextNode
            || !(lexer->insert || lexer->inode)) {
            node = lexer->token;
            GTDBG(doc,"lex-token", node);
            return node;
        }
        lexer->itoken = TY_(InsertedToken)( doc );
        node = lexer->itoken;
        GTDBG(doc,"lex-inserted", node);
        return node;
    }

    assert( !(lexer->pushed || lexer->itoken) );

    /* at start of block elements, unclosed inline
       elements are inserted into the token stream */
    if (lexer->insert || lexer->inode) {
        /*\ Issue #92: could fix by the following, but instead chose not to stack these 2
         *  if ( !(lexer->insert && (nodeIsINS(lexer->insert) || nodeIsDEL(lexer->insert))) ) {
        \*/
        lexer->token = TY_(InsertedToken)( doc );
        node = lexer->token;
        GTDBG(doc,"lex-inserted2", node);
        return node;
    }

    if (mode == CdataContent)
    {
        assert( lexer->parent != NULL );
        node = GetCDATA(doc, lexer->parent);
        GTDBG(doc,"lex-cdata", node);
        return node;
    }

    return GetTokenFromStream( doc, mode );
}