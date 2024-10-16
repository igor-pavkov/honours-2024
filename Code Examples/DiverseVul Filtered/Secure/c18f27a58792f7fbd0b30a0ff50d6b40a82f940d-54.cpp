static Node *ParseDocTypeDecl(TidyDocImpl* doc)
{
    Lexer *lexer = doc->lexer;
    int start = lexer->lexsize;
    ParseDocTypeDeclState state = DT_DOCTYPENAME;
    uint c;
    uint delim = 0;
    Bool hasfpi = yes;

    Node* node = TY_(NewNode)(lexer->allocator, lexer);
    node->type = DocTypeTag;
    node->start = lexer->txtstart;
    node->end = lexer->txtend;

    lexer->waswhite = no;

    /* todo: reset lexer->lexsize when appropriate to avoid wasting memory */

    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream)
    {
        /* convert newlines to spaces */
        if (state != DT_INTSUBSET)
            c = c == '\n' ? ' ' : c;

        /* convert white-space sequences to single space character */
        if (TY_(IsWhite)(c) && state != DT_INTSUBSET)
        {
            if (!lexer->waswhite)
            {
                TY_(AddCharToLexer)(lexer, c);
                lexer->waswhite = yes;
            }
            else
            {
                /* discard space */
                continue;
            }
        }
        else
        {
            TY_(AddCharToLexer)(lexer, c);
            lexer->waswhite = no;
        }

        switch(state)
        {
        case DT_INTERMEDIATE:
            /* determine what's next */
            if (TY_(ToUpper)(c) == 'P' || TY_(ToUpper)(c) == 'S')
            {
                start = lexer->lexsize - 1;
                state = DT_PUBLICSYSTEM;
                continue;
            }
            else if (c == '[')
            {
                start = lexer->lexsize;
                state = DT_INTSUBSET;
                continue;
            }
            else if (c == '\'' || c == '"')
            {
                start = lexer->lexsize;
                delim = c;
                state = DT_QUOTEDSTRING;
                continue;
            }
            else if (c == '>')
            {
                AttVal* si;

                node->end = --(lexer->lexsize);

                si = TY_(GetAttrByName)(node, "SYSTEM");
                if (si)
                    TY_(CheckUrl)(doc, node, si);

                if (!node->element || !IsValidXMLElemName(node->element))
                {
                    TY_(ReportError)(doc, NULL, NULL, MALFORMED_DOCTYPE);
                    TY_(FreeNode)(doc, node);
                    return NULL;
                }
#ifdef TIDY_STORE_ORIGINAL_TEXT
                StoreOriginalTextInToken(doc, node, 0);
#endif
                return node;
            }
            else
            {
                /* error */
            }
            break;
        case DT_DOCTYPENAME:
            /* read document type name */
            if (TY_(IsWhite)(c) || c == '>' || c == '[')
            {
                node->element = TY_(tmbstrndup)(doc->allocator,
                                                lexer->lexbuf + start,
                                                lexer->lexsize - start - 1);
                if (c == '>' || c == '[')
                {
                    --(lexer->lexsize);
                    TY_(UngetChar)(c, doc->docIn);
                }

                state = DT_INTERMEDIATE;
                continue;
            }
            break;
        case DT_PUBLICSYSTEM:
            /* read PUBLIC/SYSTEM */
            if (TY_(IsWhite)(c) || c == '>')
            {
                char *attname = TY_(tmbstrndup)(doc->allocator,
                                                lexer->lexbuf + start,
                                                lexer->lexsize - start - 1);
                hasfpi = !(TY_(tmbstrcasecmp)(attname, "SYSTEM") == 0);

                TidyDocFree(doc, attname);

                /* todo: report an error if SYSTEM/PUBLIC not uppercase */

                if (c == '>')
                {
                    --(lexer->lexsize);
                    TY_(UngetChar)(c, doc->docIn);
                }

                state = DT_INTERMEDIATE;
                continue;
            }
            break;
        case DT_QUOTEDSTRING:
            /* read quoted string */
            if (c == delim)
            {
                char *value = TY_(tmbstrndup)(doc->allocator,
                                              lexer->lexbuf + start,
                                              lexer->lexsize - start - 1);
                AttVal* att = TY_(AddAttribute)(doc, node, hasfpi ? "PUBLIC" : "SYSTEM", value);
                TidyDocFree(doc, value);
                att->delim = delim;
                hasfpi = no;
                state = DT_INTERMEDIATE;
                delim = 0;
                continue;
            }
            break;
        case DT_INTSUBSET:
            /* read internal subset */
            if (c == ']')
            {
                Node* subset;
                lexer->txtstart = start;
                lexer->txtend = lexer->lexsize - 1;
                subset = TY_(TextToken)(lexer);
                TY_(InsertNodeAtEnd)(node, subset);
                state = DT_INTERMEDIATE;
            }
            break;
        }
    }

    /* document type declaration not finished */
    TY_(ReportError)(doc, NULL, NULL, MALFORMED_DOCTYPE);
    TY_(FreeNode)(doc, node);
    return NULL;
}