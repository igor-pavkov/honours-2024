void TY_(FreeLexer)( TidyDocImpl* doc )
{
    Lexer *lexer = doc->lexer;
    if ( lexer )
    {
        TY_(FreeStyles)( doc );

        /* See GetToken() */
        if ( lexer->pushed || lexer->itoken )
        {
            if (lexer->pushed)
                TY_(FreeNode)( doc, lexer->itoken );
            TY_(FreeNode)( doc, lexer->token );
        }

        while ( lexer->istacksize > 0 )
            TY_(PopInline)( doc, NULL );

        TidyDocFree( doc, lexer->istack );
        TidyDocFree( doc, lexer->lexbuf );
        TidyDocFree( doc, lexer );
        doc->lexer = NULL;
    }
}