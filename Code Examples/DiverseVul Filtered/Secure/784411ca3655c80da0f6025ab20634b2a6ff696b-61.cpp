static void indent(int l)
{
    int i;
    for (i = 0; i < l; i++)
	fputs("  ", stderr);
}