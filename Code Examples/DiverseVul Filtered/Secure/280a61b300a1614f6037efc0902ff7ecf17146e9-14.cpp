static void LYHandleFIG(HTStructured * me, const BOOL *present,
			STRING2PTR value,
			int isobject,
			int imagemap,
			const char *id,
			const char *src,
			int convert,
			int start,
			BOOL *intern_flag GCC_UNUSED)
{
    if (start == TRUE) {
	me->inFIG = TRUE;
	if (me->inA) {
	    SET_SKIP_STACK(HTML_A);
	    HTML_end_element(me, HTML_A, NULL);
	}
	if (!isobject) {
	    LYEnsureDoubleSpace(me);
	    LYResetParagraphAlignment(me);
	    me->inFIGwithP = TRUE;
	} else {
	    me->inFIGwithP = FALSE;
	    HTML_put_character(me, ' ');	/* space char may be ignored */
	}
	if (non_empty(id)) {
	    if (present && convert) {
		CHECK_ID(HTML_FIG_ID);
	    } else
		LYHandleID(me, id);
	}
	me->in_word = NO;
	me->inP = FALSE;

	if (clickable_images && non_empty(src)) {
	    char *href = NULL;

	    StrAllocCopy(href, src);
	    CHECK_FOR_INTERN(*intern_flag, href);
	    LYLegitimizeHREF(me, &href, TRUE, TRUE);
	    if (*href) {
		me->CurrentA = HTAnchor_findChildAndLink(me->node_anchor,	/* Parent */
							 NULL,	/* Tag */
							 href,	/* Addresss */
							 INTERN_CHK(*intern_flag));	/* Type */
		HText_beginAnchor(me->text, me->inUnderline, me->CurrentA);
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_START_CHAR);
		HTML_put_string(me, (isobject
				     ? (imagemap
					? "(IMAGE)"
					: "(OBJECT)")
				     : "[FIGURE]"));
		if (me->inBoldH == FALSE)
		    HText_appendCharacter(me->text, LY_BOLD_END_CHAR);
		HText_endAnchor(me->text, 0);
		HTML_put_character(me, '-');
		HTML_put_character(me, ' ');	/* space char may be ignored */
		me->in_word = NO;
	    }
	    FREE(href);
	}
    } else {			/* handle end tag */
	if (me->inFIGwithP) {
	    LYEnsureDoubleSpace(me);
	} else {
	    HTML_put_character(me, ' ');	/* space char may be ignored */
	}
	LYResetParagraphAlignment(me);
	me->inFIGwithP = FALSE;
	me->inFIG = FALSE;
	change_paragraph_style(me, me->sp->style);	/* Often won't really change */
	if (me->List_Nesting_Level >= 0) {
	    UPDATE_STYLE;
	    HText_NegateLineOne(me->text);
	}
    }
}