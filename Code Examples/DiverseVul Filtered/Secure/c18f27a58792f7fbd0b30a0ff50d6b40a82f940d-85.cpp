void TY_(AddCharToLexer)( Lexer *lexer, uint c )
{
    int i, err, count = 0;
    tmbchar buf[10] = {0};
    
    err = TY_(EncodeCharToUTF8Bytes)( c, buf, NULL, &count );
    if (err)
    {
#if 0 && defined(_DEBUG)
        fprintf( stderr, "lexer UTF-8 encoding error for U+%x : ", c );
#endif
        /* replacement character 0xFFFD encoded as UTF-8 */
        buf[0] = (byte) 0xEF;
        buf[1] = (byte) 0xBF;
        buf[2] = (byte) 0xBD;
        count = 3;
    }
    
    for ( i = 0; i < count; ++i )
        AddByte( lexer, buf[i] );
}