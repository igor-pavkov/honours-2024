void HtmlPage::coalesce() {
  HtmlString *str1, *str2;
  HtmlFont *hfont1, *hfont2;
  double space, horSpace, vertSpace, vertOverlap;
  bool addSpace, addLineBreak;
  int n, i;
  double curX, curY;

#if 0 //~ for debugging
  for (str1 = yxStrings; str1; str1 = str1->yxNext) {
    printf("x=%f..%f  y=%f..%f  size=%2d '",
	   str1->xMin, str1->xMax, str1->yMin, str1->yMax,
	   (int)(str1->yMax - str1->yMin));
    for (i = 0; i < str1->len; ++i) {
      fputc(str1->text[i] & 0xff, stdout);
    }
    printf("'\n");
  }
  printf("\n------------------------------------------------------------\n\n");
#endif
  str1 = yxStrings;

  if( !str1 ) return;

  //----- discard duplicated text (fake boldface, drop shadows)
  if( !complexMode )
  {	/* if not in complex mode get rid of duplicate strings */
	HtmlString *str3;
	bool found;
  	while (str1)
	{
		double size = str1->yMax - str1->yMin;
		double xLimit = str1->xMin + size * 0.2;
		found = false;
		for (str2 = str1, str3 = str1->yxNext;
			str3 && str3->xMin < xLimit;
			str2 = str3, str3 = str2->yxNext)
		{
			if (str3->len == str1->len &&
				!memcmp(str3->text, str1->text, str1->len * sizeof(Unicode)) &&
				fabs(str3->yMin - str1->yMin) < size * 0.2 &&
				fabs(str3->yMax - str1->yMax) < size * 0.2 &&
				fabs(str3->xMax - str1->xMax) < size * 0.2)
			{
				found = true;
				//printf("found duplicate!\n");
				break;
			}
		}
		if (found)
		{
			str2->xyNext = str3->xyNext;
			str2->yxNext = str3->yxNext;
			delete str3;
		}
		else
		{
			str1 = str1->yxNext;
		}
	}		
  }	/*- !complexMode */
  
  str1 = yxStrings;
  
  hfont1 = getFont(str1);
  if( hfont1->isBold() )
    str1->htext->insert(0,"<b>",3);
  if( hfont1->isItalic() )
    str1->htext->insert(0,"<i>",3);
  if( str1->getLink() != nullptr ) {
    GooString *ls = str1->getLink()->getLinkStart();
    str1->htext->insert(0, ls);
    delete ls;
  }
  curX = str1->xMin; curY = str1->yMin;

  while (str1 && (str2 = str1->yxNext)) {
    hfont2 = getFont(str2);
    space = str1->yMax - str1->yMin; // the height of the font's bounding box
    horSpace = str2->xMin - str1->xMax;
    // if strings line up on left-hand side AND they are on subsequent lines, we need a line break
    addLineBreak = !noMerge && (fabs(str1->xMin - str2->xMin) < 0.4) && IS_CLOSER(str2->yMax, str1->yMax + space, str1->yMax);
    vertSpace = str2->yMin - str1->yMax;

//printf("coalesce %d %d %f? ", str1->dir, str2->dir, d);

    if (str2->yMin >= str1->yMin && str2->yMin <= str1->yMax)
    {
	vertOverlap = str1->yMax - str2->yMin;
    } else
    if (str2->yMax >= str1->yMin && str2->yMax <= str1->yMax)
    {
	vertOverlap = str2->yMax - str1->yMin;
    } else
    {
    	vertOverlap = 0;
    } 
    
    // Combine strings if:
    //  They appear to be the same font (complex mode only) && going in the same direction AND at least one of the following:
    //  1.  They appear to be part of the same line of text
    //  2.  They appear to be subsequent lines of a paragraph
    //  We assume (1) or (2) above, respectively, based on:
    //  (1)  strings overlap vertically AND
    //       horizontal space between end of str1 and start of str2 is consistent with a single space or less;
    //       when rawOrder, the strings have to overlap vertically by at least 50%
    //  (2)  Strings flow down the page, but the space between them is not too great, and they are lined up on the left
    if (
	(
	 (
	  (
	   (rawOrder && vertOverlap > 0.5 * space) 
	   ||
	   (!rawOrder && str2->yMin < str1->yMax)
	  ) &&
	  (horSpace > -0.5 * space && horSpace < space)
	 ) ||
       	 (vertSpace >= 0 && vertSpace < 0.5 * space && addLineBreak)
	) &&
	(!complexMode || (hfont1->isEqualIgnoreBold(*hfont2))) && // in complex mode fonts must be the same, in other modes fonts do not metter
	str1->dir == str2->dir // text direction the same
       ) 
    {
//      printf("yes\n");
      n = str1->len + str2->len;
      if ((addSpace = horSpace > wordBreakThreshold * space)) {
        ++n;
      }
      if (addLineBreak) {
        ++n;
      }
  
      str1->size = (n + 15) & ~15;
      str1->text = (Unicode *)grealloc(str1->text,
				       str1->size * sizeof(Unicode));
      str1->xRight = (double *)grealloc(str1->xRight,
					str1->size * sizeof(double));
      if (addSpace) {
		  str1->text[str1->len] = 0x20;
		  str1->htext->append(xml?" ":"&#160;");
		  str1->xRight[str1->len] = str2->xMin;
		  ++str1->len;
      }
      if (addLineBreak) {
	  str1->text[str1->len] = '\n';
	  str1->htext->append("<br/>");
	  str1->xRight[str1->len] = str2->xMin;
	  ++str1->len;
	  str1->yMin = str2->yMin;
	  str1->yMax = str2->yMax;
	  str1->xMax = str2->xMax;
	  int fontLineSize = hfont1->getLineSize();
	  int curLineSize = (int)(vertSpace + space); 
	  if( curLineSize != fontLineSize )
	  {
	      HtmlFont *newfnt = new HtmlFont(*hfont1);
	      newfnt->setLineSize(curLineSize);
	      str1->fontpos = fonts->AddFont(*newfnt);
	      delete newfnt;
	      hfont1 = getFont(str1);
	      // we have to reget hfont2 because it's location could have
	      // changed on resize
	      hfont2 = getFont(str2); 
	  }
      }
      for (i = 0; i < str2->len; ++i) {
	str1->text[str1->len] = str2->text[i];
	str1->xRight[str1->len] = str2->xRight[i];
	++str1->len;
      }

      /* fix <i>, <b> if str1 and str2 differ and handle switch of links */
      HtmlLink *hlink1 = str1->getLink();
      HtmlLink *hlink2 = str2->getLink();
      bool switch_links = !hlink1 || !hlink2 || !hlink1->isEqualDest(*hlink2);
      bool finish_a = switch_links && hlink1 != nullptr;
      bool finish_italic = hfont1->isItalic() && ( !hfont2->isItalic() || finish_a );
      bool finish_bold   = hfont1->isBold()   && ( !hfont2->isBold()   || finish_a || finish_italic );
      CloseTags( str1->htext, finish_a, finish_italic, finish_bold );
      if( switch_links && hlink2 != nullptr ) {
        GooString *ls = hlink2->getLinkStart();
        str1->htext->append(ls);
        delete ls;
      }
      if( ( !hfont1->isItalic() || finish_italic ) && hfont2->isItalic() )
	    str1->htext->append("<i>", 3);
      if( ( !hfont1->isBold() || finish_bold ) && hfont2->isBold() )
	    str1->htext->append("<b>", 3);


      str1->htext->append(str2->htext);
      // str1 now contains href for link of str2 (if it is defined)
      str1->link = str2->link; 
      hfont1 = hfont2;
      if (str2->xMax > str1->xMax) {
	str1->xMax = str2->xMax;
      }
      if (str2->yMax > str1->yMax) {
	str1->yMax = str2->yMax;
      }
      str1->yxNext = str2->yxNext;
      delete str2;
    } else { // keep strings separate
//      printf("no\n"); 
      bool finish_a = str1->getLink() != nullptr;
      bool finish_bold   = hfont1->isBold();
      bool finish_italic = hfont1->isItalic();
      CloseTags( str1->htext, finish_a, finish_italic, finish_bold );
     
      str1->xMin = curX; str1->yMin = curY; 
      str1 = str2;
      curX = str1->xMin; curY = str1->yMin;
      hfont1 = hfont2;
      if( hfont1->isBold() )
	str1->htext->insert(0,"<b>",3);
      if( hfont1->isItalic() )
	str1->htext->insert(0,"<i>",3);
      if( str1->getLink() != nullptr ) {
	GooString *ls = str1->getLink()->getLinkStart();
	str1->htext->insert(0, ls);
	delete ls;
      }
    }
  }
  str1->xMin = curX; str1->yMin = curY;

  bool finish_bold   = hfont1->isBold();
  bool finish_italic = hfont1->isItalic();
  bool finish_a = str1->getLink() != nullptr;
  CloseTags( str1->htext, finish_a, finish_italic, finish_bold );

#if 0 //~ for debugging
  for (str1 = yxStrings; str1; str1 = str1->yxNext) {
    printf("x=%3d..%3d  y=%3d..%3d  size=%2d ",
	   (int)str1->xMin, (int)str1->xMax, (int)str1->yMin, (int)str1->yMax,
	   (int)(str1->yMax - str1->yMin));
    printf("'%s'\n", str1->htext->c_str());  
  }
  printf("\n------------------------------------------------------------\n\n");
#endif

}