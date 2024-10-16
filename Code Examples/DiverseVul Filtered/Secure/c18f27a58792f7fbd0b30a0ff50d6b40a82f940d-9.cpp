static void SetLexerLocus( TidyDocImpl* doc, Lexer *lexer )
{
    lexer->lines = doc->docIn->curline;
    lexer->columns = doc->docIn->curcol;
}