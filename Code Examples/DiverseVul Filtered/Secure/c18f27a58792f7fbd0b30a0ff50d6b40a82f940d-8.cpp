void TY_(UngetToken)( TidyDocImpl* doc )
{
    doc->lexer->pushed = yes;
}