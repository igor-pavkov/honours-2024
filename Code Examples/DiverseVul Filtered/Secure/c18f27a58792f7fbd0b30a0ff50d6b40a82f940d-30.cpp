static void Show_Node( TidyDocImpl* doc, const char *msg, Node *node )
{
    Lexer* lexer = doc->lexer;
    Bool lex = ((msg[0] == 'l')&&(msg[1] == 'e')) ? yes : no;
    int line = ( doc->lexer ? doc->lexer->lines : 0 );
    int col  = ( doc->lexer ? doc->lexer->columns : 0 );
    SPRTF("R=%d C=%d: ", line, col );
    // DEBUG: Be able to set a TRAP on a SPECIFIC row,col
    if ((line == 6) && (col == 33)) {
        check_me("Show_Node"); // just a debug trap
    }
    if (lexer && lexer->token && (lexer->token->type == TextNode)) {
        if (show_attrs) {
            uint len = node->end - node->start;
            tmbstr cp = get_text_string( lexer, node );
            SPRTF("Returning %s TextNode [%s]%u %s\n", msg, cp, len,
                lex ? "lexer" : "stream");
        } else {
            SPRTF("Returning %s TextNode %p... %s\n", msg, node,
                lex ? "lexer" : "stream");
        }
    } else {
        if (show_attrs) {
            AttVal* av;
            tmbstr name = node->element ? node->element : "blank";
            SPRTF("Returning %s node <%s", msg, name);
            for (av = node->attributes; av; av = av->next) {
                name = av->attribute;
                if (name) {
                    SPRTF(" %s",name);
                    if (av->value) {
                        SPRTF("=\"%s\"", av->value);
                    }
                }
            }
            SPRTF("> %s\n", lex ? "lexer" : "stream");
        } else {
            SPRTF("Returning %s node %p <%s>... %s\n", msg, node,
                node->element ? node->element : "blank",
                lex ? "lexer" : "stream");
        }
    }
}