static field_t *parse_reclbl(node_t * n, int LR, int flag, char *text)
{
    field_t *fp, *rv = NEW(field_t);
    char *tsp, *psp=NULL, *hstsp, *hspsp=NULL, *sp;
    char *tmpport = NULL;
    int maxf, cnt, mode, wflag, ishardspace, fi;
    textlabel_t *lbl = ND_label(n);
    unsigned char uc;

    fp = NULL;
    for (maxf = 1, cnt = 0, sp = reclblp; *sp; sp++) {
	if (*sp == '\\') {
	    sp++;
	    if (*sp
		&& (*sp == '{' || *sp == '}' || *sp == '|' || *sp == '\\'))
		continue;
	}
	if (*sp == '{')
	    cnt++;
	else if (*sp == '}')
	    cnt--;
	else if (*sp == '|' && cnt == 0)
	    maxf++;
	if (cnt < 0)
	    break;
    }
    rv->fld = N_NEW(maxf, field_t *);
    rv->LR = LR;
    mode = 0;
    fi = 0;
    hstsp = tsp = text;
    wflag = TRUE;
    ishardspace = FALSE;
    while (wflag) {
	if ((uc = *(unsigned char*)reclblp) && (uc < ' ')) {    /* Ignore non-0 control characters */
	    reclblp++;
	    continue;
	}
	switch (*reclblp) {
	case '<':
	    if (mode & (HASTABLE | HASPORT))
		return parse_error(rv, tmpport);
	    if (lbl->html)
		goto dotext;
	    mode |= (HASPORT | INPORT);
	    reclblp++;
	    hspsp = psp = text;
	    break;
	case '>':
	    if (lbl->html)
		goto dotext;
	    if (!(mode & INPORT))
		return parse_error(rv, tmpport);
	    if (psp > text + 1 && psp - 1 != hspsp && *(psp - 1) == ' ')
		psp--;
	    *psp = '\000';
	    tmpport = strdup(text);
	    mode &= ~INPORT;
	    reclblp++;
	    break;
	case '{':
	    reclblp++;
	    if (mode != 0 || !*reclblp)
		return parse_error(rv, tmpport);
	    mode = HASTABLE;
	    if (!(rv->fld[fi++] = parse_reclbl(n, NOT(LR), FALSE, text)))
		return parse_error(rv, tmpport);
	    break;
	case '}':
	case '|':
	case '\000':
	    if ((!*reclblp && !flag) || (mode & INPORT))
		return parse_error(rv, tmpport);
	    if (!(mode & HASTABLE))
		fp = rv->fld[fi++] = NEW(field_t);
	    if (tmpport) {
		fp->id = tmpport;
		tmpport = NULL;
	    }
	    if (!(mode & (HASTEXT | HASTABLE)))
		mode |= HASTEXT, *tsp++ = ' ';
	    if (mode & HASTEXT) {
		if (tsp > text + 1 &&
		    tsp - 1 != hstsp && *(tsp - 1) == ' ')
		    tsp--;
		*tsp = '\000';
		fp->lp =
		    make_label((void *) n, strdup(text),
			       (lbl->html ? LT_HTML : LT_NONE),
			       lbl->fontsize, lbl->fontname,
			       lbl->fontcolor);
		fp->LR = TRUE;
		hstsp = tsp = text;
	    }
	    if (*reclblp) {
		if (*reclblp == '}') {
		    reclblp++;
		    rv->n_flds = fi;
		    return rv;
		}
		mode = 0;
		reclblp++;
	    } else
		wflag = FALSE;
	    break;
	case '\\':
	    if (*(reclblp + 1)) {
		if (ISCTRL(*(reclblp + 1)))
		    reclblp++;
		else if ((*(reclblp + 1) == ' ') && !lbl->html)
		    ishardspace = TRUE, reclblp++;
		else {
		    *tsp++ = '\\';
		    mode |= (INTEXT | HASTEXT);
		    reclblp++;
		}
	    }
	    /* falling through ... */
	default:
	  dotext:
	    if ((mode & HASTABLE) && *reclblp != ' ')
		return parse_error(rv, tmpport);
	    if (!(mode & (INTEXT | INPORT)) && *reclblp != ' ')
		mode |= (INTEXT | HASTEXT);
	    if (mode & INTEXT) {
		if (!
		    (*reclblp == ' ' && !ishardspace && *(tsp - 1) == ' '
		     && !lbl->html))
		    *tsp++ = *reclblp;
		if (ishardspace)
		    hstsp = tsp - 1;
	    } else if (mode & INPORT) {
		if (!(*reclblp == ' ' && !ishardspace &&
		      (psp == text || *(psp - 1) == ' ')))
		    *psp++ = *reclblp;
		if (ishardspace)
		    hspsp = psp - 1;
	    }
	    reclblp++;
	    while (*reclblp & 128)
		*tsp++ = *reclblp++;
	    break;
	}
    }
    rv->n_flds = fi;
    return rv;
}