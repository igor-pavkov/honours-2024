int FoFiTrueType::checkGIDInCoverage(Guint coverage, Guint orgGID)
{
  int index = -1;
  Guint format;
  Guint count;
  Guint i;
  Guint pos;

  pos = coverage;
  format = getU16BE(pos,&parsedOk);
  pos += 2;
  switch (format) {
  case 1:
    count = getU16BE(pos,&parsedOk);
    pos += 2;
    // In some poor CJK fonts, key GIDs are not sorted,
    // thus we cannot finish checking even when the range
    // including orgGID seems to have already passed.
    for (i = 0;i < count;i++) {
      Guint gid;

      gid = getU16BE(pos,&parsedOk);
      pos += 2;
      if (gid == orgGID) {
        /* found */
        index = i;
        break;
      }
    }
    break;
  case 2:
    count = getU16BE(pos,&parsedOk);
    pos += 2;
    for (i = 0;i < count;i++) {
      Guint startGID, endGID;
      Guint startIndex;

      startGID = getU16BE(pos,&parsedOk);
      pos += 2;
      endGID = getU16BE(pos,&parsedOk);
      pos += 2;
      startIndex = getU16BE(pos,&parsedOk);
      pos += 2;
      // In some poor CJK fonts, key GIDs are not sorted,
      // thus we cannot finish checking even when the range
      // including orgGID seems to have already passed.
      if (startGID <= orgGID && orgGID <= endGID) {
        /* found */
        index = startIndex+orgGID-startGID;
        break;
      }
    }
    break;
  default:
    break;
  }
  return index;
}