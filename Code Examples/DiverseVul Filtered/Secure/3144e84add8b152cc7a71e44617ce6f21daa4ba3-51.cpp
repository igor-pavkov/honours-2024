static void free_tlist(Token * list)
{
    while (list)
        list = delete_Token(list);
}