void TY_(AddStringLiteral)( Lexer* lexer, ctmbstr str )
{
    byte c;
    while(0 != (c = *str++) )
        TY_(AddCharToLexer)( lexer, c );
}