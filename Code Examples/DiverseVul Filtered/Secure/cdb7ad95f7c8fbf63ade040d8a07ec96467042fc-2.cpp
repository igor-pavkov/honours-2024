void FoFiTrueType::readPostTable() {
  GooString *name;
  int tablePos, postFmt, stringIdx, stringPos, savedStringIdx;
  GBool ok;
  int i, j, n, m;

  ok = gTrue;
  if ((i = seekTable("post")) < 0) {
    return;
  }
  tablePos = tables[i].offset;
  postFmt = getU32BE(tablePos, &ok);
  if (!ok) {
    goto err;
  }
  if (postFmt == 0x00010000) {
    nameToGID = new GooHash(gTrue);
    for (i = 0; i < 258; ++i) {
      nameToGID->add(new GooString(macGlyphNames[i]), i);
    }
  } else if (postFmt == 0x00020000) {
    nameToGID = new GooHash(gTrue);
    n = getU16BE(tablePos + 32, &ok);
    if (!ok) {
      goto err;
    }
    if (n > nGlyphs) {
      n = nGlyphs;
    }
    stringIdx = 0;
    stringPos = tablePos + 34 + 2*n;
    for (i = 0; i < n; ++i) {
      j = getU16BE(tablePos + 34 + 2*i, &ok);
      if (j < 258) {
	nameToGID->removeInt(macGlyphNames[j]);
	nameToGID->add(new GooString(macGlyphNames[j]), i);
      } else {
	savedStringIdx = stringIdx;
	j -= 258;
	if (j != stringIdx) {
	  for (stringIdx = 0, stringPos = tablePos + 34 + 2*n;
	       stringIdx < j;
	       ++stringIdx, stringPos += 1 + getU8(stringPos, &ok)) ;
	  if (!ok) {
	    goto err;
	  }
	}
	m = getU8(stringPos, &ok);
	if (!ok || !checkRegion(stringPos + 1, m)) {
	  stringIdx = savedStringIdx;
	  if (j < 258) {
	    ok = gTrue;
	    nameToGID->removeInt(macGlyphNames[j]);
	    nameToGID->add(new GooString(macGlyphNames[0]), i);
	  } else {
	    goto err;
	  }
	} else {
	  name = new GooString((char *)&file[stringPos + 1], m);
	  nameToGID->removeInt(name);
	  nameToGID->add(name, i);
	  ++stringIdx;
	  stringPos += 1 + m;
        }
      }
    }
  } else if (postFmt == 0x00028000) {
    nameToGID = new GooHash(gTrue);
    for (i = 0; i < nGlyphs; ++i) {
      j = getU8(tablePos + 32 + i, &ok);
      if (!ok) {
	goto err;
      }
      if (j < 258) {
	nameToGID->removeInt(macGlyphNames[j]);
	nameToGID->add(new GooString(macGlyphNames[j]), i);
      }
    }
  }

  return;

 err:
  if (nameToGID) {
    delete nameToGID;
    nameToGID = NULL;
  }
}