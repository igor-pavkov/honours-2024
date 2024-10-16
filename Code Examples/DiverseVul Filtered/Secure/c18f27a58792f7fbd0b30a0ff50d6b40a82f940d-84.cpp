static tmbstr get_text_string(Lexer* lexer, Node *node)
{
    uint len = node->end - node->start;
    tmbstr cp = lexer->lexbuf + node->start;
    tmbstr end = lexer->lexbuf + node->end;
    uint i = 0;
    buffer[0] = (char)0;
    while (cp < end ) {
        buffer[i++] = *cp;
        cp++;
        if (i >= MX_TXT)
            break;
    }
    if (i < len) {
        buffer[i++] = '.';
        if (i < len) {
            buffer[i++] = '.';
            if (i < len) {
                buffer[i++] = '.';
            }
        }
    }
    buffer[i] = 0;
    return buffer;
}