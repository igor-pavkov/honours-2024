static void get_styles(void)
{
    HTStyle **st = NULL;

    styleSheet = DefaultStyle(&st);	/* sets st[] array */

    default_style = st[ST_Normal];

    styles[HTML_H1] = st[ST_Heading1];
    styles[HTML_H2] = st[ST_Heading2];
    styles[HTML_H3] = st[ST_Heading3];
    styles[HTML_H4] = st[ST_Heading4];
    styles[HTML_H5] = st[ST_Heading5];
    styles[HTML_H6] = st[ST_Heading6];
    styles[HTML_HCENTER] = st[ST_HeadingCenter];
    styles[HTML_HLEFT] = st[ST_HeadingLeft];
    styles[HTML_HRIGHT] = st[ST_HeadingRight];

    styles[HTML_DCENTER] = st[ST_DivCenter];
    styles[HTML_DLEFT] = st[ST_DivLeft];
    styles[HTML_DRIGHT] = st[ST_DivRight];

    styles[HTML_DL] = st[ST_Glossary];
    /* nested list styles */
    styles[HTML_DL1] = st[ST_Glossary1];
    styles[HTML_DL2] = st[ST_Glossary2];
    styles[HTML_DL3] = st[ST_Glossary3];
    styles[HTML_DL4] = st[ST_Glossary4];
    styles[HTML_DL5] = st[ST_Glossary5];
    styles[HTML_DL6] = st[ST_Glossary6];

    styles[HTML_UL] =
	styles[HTML_OL] = st[ST_List];
    /* nested list styles */
    styles[HTML_OL1] = st[ST_List1];
    styles[HTML_OL2] = st[ST_List2];
    styles[HTML_OL3] = st[ST_List3];
    styles[HTML_OL4] = st[ST_List4];
    styles[HTML_OL5] = st[ST_List5];
    styles[HTML_OL6] = st[ST_List6];

    styles[HTML_MENU] =
	styles[HTML_DIR] = st[ST_Menu];
    /* nested list styles */
    styles[HTML_MENU1] = st[ST_Menu1];
    styles[HTML_MENU2] = st[ST_Menu2];
    styles[HTML_MENU3] = st[ST_Menu3];
    styles[HTML_MENU4] = st[ST_Menu4];
    styles[HTML_MENU5] = st[ST_Menu5];
    styles[HTML_MENU6] = st[ST_Menu6];

    styles[HTML_DLC] = st[ST_GlossaryCompact];
    /* nested list styles */
    styles[HTML_DLC1] = st[ST_GlossaryCompact1];
    styles[HTML_DLC2] = st[ST_GlossaryCompact2];
    styles[HTML_DLC3] = st[ST_GlossaryCompact3];
    styles[HTML_DLC4] = st[ST_GlossaryCompact4];
    styles[HTML_DLC5] = st[ST_GlossaryCompact5];
    styles[HTML_DLC6] = st[ST_GlossaryCompact6];

    styles[HTML_ADDRESS] = st[ST_Address];
    styles[HTML_BANNER] = st[ST_Banner];
    styles[HTML_BLOCKQUOTE] = st[ST_Blockquote];
    styles[HTML_BQ] = st[ST_Bq];
    styles[HTML_FN] = st[ST_Footnote];
    styles[HTML_NOTE] = st[ST_Note];
    styles[HTML_PLAINTEXT] =
	styles[HTML_XMP] = st[ST_Example];
    styles[HTML_PRE] = st[ST_Preformatted];
    styles[HTML_LISTING] = st[ST_Listing];
}