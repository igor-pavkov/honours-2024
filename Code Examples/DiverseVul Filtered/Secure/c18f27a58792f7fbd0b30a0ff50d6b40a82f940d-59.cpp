static void ParseEntity( TidyDocImpl* doc, GetTokenMode mode )
{
    typedef enum
    {
        ENT_default,
        ENT_numdec,
        ENT_numhex
    } ENTState;
    
    typedef Bool (*ENTfn)(uint);
    const ENTfn entFn[] = {
        TY_(IsNamechar),
        TY_(IsDigit),
        IsDigitHex
    };
    uint start;
    ENTState entState = ENT_default;
    uint charRead = 0;
    Bool semicolon = no, found = no;
    Bool isXml = cfgBool( doc, TidyXmlTags );
    Bool preserveEntities = cfgBool( doc, TidyPreserveEntities );
    uint c, ch, startcol, entver = 0;
    Lexer* lexer = doc->lexer;

    start = lexer->lexsize - 1;  /* to start at "&" */
    startcol = doc->docIn->curcol - 1;

    while ( (c = TY_(ReadChar)(doc->docIn)) != EndOfStream )
    {
        if ( c == ';' )
        {
            semicolon = yes;
            break;
        }
        ++charRead;

        if (charRead == 1 && c == '#')
        {
#if SUPPORT_ASIAN_ENCODINGS
            if ( !cfgBool(doc, TidyNCR) || 
                 cfg(doc, TidyInCharEncoding) == BIG5 ||
                 cfg(doc, TidyInCharEncoding) == SHIFTJIS )
            {
                TY_(UngetChar)('#', doc->docIn);
                return;
            }
#endif
            TY_(AddCharToLexer)( lexer, c );
            entState = ENT_numdec;
            continue;
        }
        else if (charRead == 2 && entState == ENT_numdec
                 && (c == 'x' || (!isXml && c == 'X')) )
        {
            TY_(AddCharToLexer)( lexer, c );
            entState = ENT_numhex;
            continue;
        }

        if ( entFn[entState](c) )
        {
            TY_(AddCharToLexer)( lexer, c );
            continue;
        }

        /* otherwise put it back */
        TY_(UngetChar)( c, doc->docIn );
        break;
    }

    /* make sure entity is NULL terminated */
    lexer->lexbuf[lexer->lexsize] = '\0';

    /* Should contrain version to XML/XHTML if &apos; 
    ** is encountered.  But this is not possible with
    ** Tidy's content model bit mask.
    */
    if ( TY_(tmbstrcmp)(lexer->lexbuf+start, "&apos") == 0
         && !cfgBool(doc, TidyXmlOut)
         && !lexer->isvoyager
         && !cfgBool(doc, TidyXhtmlOut) )
        TY_(ReportEntityError)( doc, APOS_UNDEFINED, lexer->lexbuf+start, 39 );

    if (( mode == OtherNamespace ) && ( c == ';' ))
    {
        /* #130 MathML attr and entity fix! */
        found = yes;
        ch = 255;
        entver = XH50|HT50;
        preserveEntities = yes;
    }
    else
    {
        /* Lookup entity code and version
        */
        found = TY_(EntityInfo)( lexer->lexbuf+start, isXml, &ch, &entver );
    }

    /* deal with unrecognized or invalid entities */
    /* #433012 - fix by Randy Waki 17 Feb 01 */
    /* report invalid NCR's - Terry Teague 01 Sep 01 */
    if ( !found || (ch >= 128 && ch <= 159) || (ch >= 256 && c != ';') )
    {
        /* set error position just before offending character */
        SetLexerLocus( doc, lexer );
        lexer->columns = startcol;

        if (lexer->lexsize > start + 1)
        {
            if (ch >= 128 && ch <= 159)
            {
                /* invalid numeric character reference */
                
                uint c1 = 0;
                int replaceMode = DISCARDED_CHAR;
            
                if ( TY_(ReplacementCharEncoding) == WIN1252 )
                    c1 = TY_(DecodeWin1252)( ch );
                else if ( TY_(ReplacementCharEncoding) == MACROMAN )
                    c1 = TY_(DecodeMacRoman)( ch );

                if ( c1 )
                    replaceMode = REPLACED_CHAR;
                
                if ( c != ';' )  /* issue warning if not terminated by ';' */
                    TY_(ReportEntityError)( doc, MISSING_SEMICOLON_NCR,
                                            lexer->lexbuf+start, c );
 
                TY_(ReportEncodingError)(doc, INVALID_NCR, ch, replaceMode == DISCARDED_CHAR);
                
                if ( c1 )
                {
                    /* make the replacement */
                    lexer->lexsize = start;
                    TY_(AddCharToLexer)( lexer, c1 );
                    semicolon = no;
                }
                else
                {
                    /* discard */
                    lexer->lexsize = start;
                    semicolon = no;
               }
               
            }
            else
                TY_(ReportEntityError)( doc, UNKNOWN_ENTITY,
                                        lexer->lexbuf+start, ch );

            if (semicolon)
                TY_(AddCharToLexer)( lexer, ';' );
        }
        else /* naked & */
            TY_(ReportEntityError)( doc, UNESCAPED_AMPERSAND,
                                    lexer->lexbuf+start, ch );
    }
    else
    {
        if ( c != ';' )    /* issue warning if not terminated by ';' */
        {
            /* set error position just before offending chararcter */
            SetLexerLocus( doc, lexer );
            lexer->columns = startcol;
            TY_(ReportEntityError)( doc, MISSING_SEMICOLON, lexer->lexbuf+start, c );
        }

        if (preserveEntities)
            TY_(AddCharToLexer)( lexer, ';' );
        else
        {
            lexer->lexsize = start;
            if ( ch == 160 && (mode == Preformatted) )
                ch = ' ';
            TY_(AddCharToLexer)( lexer, ch );

            if ( ch == '&' && !cfgBool(doc, TidyQuoteAmpersand) )
                AddStringToLexer( lexer, "amp;" );
        }

        /* Detect extended vs. basic entities */
        TY_(ConstrainVersion)( doc, entver );
    }
}