void FoFiTrueType::parse() {
  Guint topTag;
  int pos, ver, i, j;

  parsedOk = gTrue;

  // look for a collection (TTC)
  topTag = getU32BE(0, &parsedOk);
  if (!parsedOk) {
    return;
  }
  if (topTag == ttcfTag) {
    /* TTC font */
    int dircount;

    dircount = getU32BE(8, &parsedOk);
    if (!parsedOk)
      return;
    if (! dircount) {
      parsedOk = gFalse;
      return;
    }

    if (faceIndex >= dircount)
      faceIndex = 0;
    pos = getU32BE(12 + faceIndex * 4, &parsedOk);
    if (! parsedOk)
      return;
  } else {
    pos = 0;
  }

  // check the sfnt version
  ver = getU32BE(pos, &parsedOk);
  if (!parsedOk) {
    return;
  }
  openTypeCFF = ver == 0x4f54544f; // 'OTTO'

  // read the table directory
  nTables = getU16BE(pos + 4, &parsedOk);
  if (!parsedOk) {
    return;
  }
  tables = (TrueTypeTable *)gmallocn(nTables, sizeof(TrueTypeTable));
  pos += 12;
  j = 0;
  for (i = 0; i < nTables; ++i) {
    tables[j].tag = getU32BE(pos, &parsedOk);
    tables[j].checksum = getU32BE(pos + 4, &parsedOk);
    tables[j].offset = (int)getU32BE(pos + 8, &parsedOk);
    tables[j].len = (int)getU32BE(pos + 12, &parsedOk);
    if (tables[j].offset + tables[j].len >= tables[j].offset &&
	tables[j].offset + tables[j].len <= len) {
      // ignore any bogus entries in the table directory
      ++j;
    }
    pos += 16;
  }
  if (nTables != j) {
    nTables = j;
    tables = (TrueTypeTable *)greallocn_checkoverflow(tables, nTables, sizeof(TrueTypeTable));
  }
  if (!parsedOk || tables == NULL) {
    return;
  }

  // check for tables that are required by both the TrueType spec and
  // the Type 42 spec
  if (seekTable("head") < 0 ||
      seekTable("hhea") < 0 ||
      seekTable("maxp") < 0 ||
      (!openTypeCFF && seekTable("loca") < 0) ||
      (!openTypeCFF && seekTable("glyf") < 0) ||
      (openTypeCFF && seekTable("CFF ") < 0)) {
    parsedOk = gFalse;
    return;
  }

  // read the cmaps
  if ((i = seekTable("cmap")) >= 0) {
    pos = tables[i].offset + 2;
    nCmaps = getU16BE(pos, &parsedOk);
    pos += 2;
    if (!parsedOk) {
      return;
    }
    cmaps = (TrueTypeCmap *)gmallocn(nCmaps, sizeof(TrueTypeCmap));
    for (j = 0; j < nCmaps; ++j) {
      cmaps[j].platform = getU16BE(pos, &parsedOk);
      cmaps[j].encoding = getU16BE(pos + 2, &parsedOk);
      cmaps[j].offset = tables[i].offset + getU32BE(pos + 4, &parsedOk);
      pos += 8;
      cmaps[j].fmt = getU16BE(cmaps[j].offset, &parsedOk);
      cmaps[j].len = getU16BE(cmaps[j].offset + 2, &parsedOk);
    }
    if (!parsedOk) {
      return;
    }
  } else {
    nCmaps = 0;
  }

  // get the number of glyphs from the maxp table
  i = seekTable("maxp");
  nGlyphs = getU16BE(tables[i].offset + 4, &parsedOk);
  if (!parsedOk) {
    return;
  }

  // get the bbox and loca table format from the head table
  i = seekTable("head");
  bbox[0] = getS16BE(tables[i].offset + 36, &parsedOk);
  bbox[1] = getS16BE(tables[i].offset + 38, &parsedOk);
  bbox[2] = getS16BE(tables[i].offset + 40, &parsedOk);
  bbox[3] = getS16BE(tables[i].offset + 42, &parsedOk);
  locaFmt = getS16BE(tables[i].offset + 50, &parsedOk);
  if (!parsedOk) {
    return;
  }

  // read the post table
  readPostTable();
}