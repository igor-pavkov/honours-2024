Guint FoFiTrueType::scanLookupSubTable(Guint subTable, Guint orgGID)
{
  Guint format;
  Guint coverage;
  int delta;
  int glyphCount;
  Guint substitute;
  Guint gid = 0;
  int coverageIndex;
  int pos;

  pos = subTable;
  format = getU16BE(pos,&parsedOk);
  pos += 2;
  coverage = getU16BE(pos,&parsedOk);
  pos += 2;
  if ((coverageIndex =
     checkGIDInCoverage(subTable+coverage,orgGID)) >= 0) {
    switch (format) {
    case 1:
      /* format 1 */
      delta = getS16BE(pos,&parsedOk);
      pos += 2;
      gid = orgGID+delta;
      break;
    case 2:
      /* format 2 */
      glyphCount = getS16BE(pos,&parsedOk);
      pos += 2;
      if (glyphCount > coverageIndex) {
	pos += coverageIndex*2;
	substitute = getU16BE(pos,&parsedOk);
        gid = substitute;
      }
      break;
    default:
      /* unknown format */
      break;
    }
  }
  return gid;
}