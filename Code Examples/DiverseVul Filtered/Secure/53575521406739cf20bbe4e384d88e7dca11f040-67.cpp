put_escstr(FILE *fd, char_u *strstart, int what)
{
    char_u	*str = strstart;
    int		c;
    int		modifiers;

    /* :map xx <Nop> */
    if (*str == NUL && what == 1)
    {
	if (fprintf(fd, "<Nop>") < 0)
	    return FAIL;
	return OK;
    }

    for ( ; *str != NUL; ++str)
    {
	char_u	*p;

	/* Check for a multi-byte character, which may contain escaped
	 * K_SPECIAL and CSI bytes */
	p = mb_unescape(&str);
	if (p != NULL)
	{
	    while (*p != NUL)
		if (fputc(*p++, fd) < 0)
		    return FAIL;
	    --str;
	    continue;
	}

	c = *str;
	/*
	 * Special key codes have to be translated to be able to make sense
	 * when they are read back.
	 */
	if (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0x0;
	    if (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;
		c = *str;
	    }
	    if (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    if (IS_SPECIAL(c) || modifiers)	/* special key */
	    {
		if (fputs((char *)get_special_key_name(c, modifiers), fd) < 0)
		    return FAIL;
		continue;
	    }
	}

	/*
	 * A '\n' in a map command should be written as <NL>.
	 * A '\n' in a set command should be written as \^V^J.
	 */
	if (c == NL)
	{
	    if (what == 2)
	    {
		if (fprintf(fd, IF_EB("\\\026\n", "\\" CTRL_V_STR "\n")) < 0)
		    return FAIL;
	    }
	    else
	    {
		if (fprintf(fd, "<NL>") < 0)
		    return FAIL;
	    }
	    continue;
	}

	/*
	 * Some characters have to be escaped with CTRL-V to
	 * prevent them from misinterpreted in DoOneCmd().
	 * A space, Tab and '"' has to be escaped with a backslash to
	 * prevent it to be misinterpreted in do_set().
	 * A space has to be escaped with a CTRL-V when it's at the start of a
	 * ":map" rhs.
	 * A '<' has to be escaped with a CTRL-V to prevent it being
	 * interpreted as the start of a special key name.
	 * A space in the lhs of a :map needs a CTRL-V.
	 */
	if (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    if (putc('\\', fd) < 0)
		return FAIL;
	}
	else if (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    if (putc(Ctrl_V, fd) < 0)
		return FAIL;
	}
	if (putc(c, fd) < 0)
	    return FAIL;
    }
    return OK;
}