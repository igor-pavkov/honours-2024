int FoFiTrueType::mapCodeToGID(int i, Guint c) {
  int gid;
  Guint segCnt, segEnd, segStart, segDelta, segOffset;
  Guint cmapFirst, cmapLen;
  int pos, a, b, m;
  GBool ok;

  if (i < 0 || i >= nCmaps) {
    return 0;
  }
  ok = gTrue;
  pos = cmaps[i].offset;
  switch (cmaps[i].fmt) {
  case 0:
    if (c + 6 >= (Guint)cmaps[i].len) {
      return 0;
    }
    gid = getU8(cmaps[i].offset + 6 + c, &ok);
    break;
  case 4:
    segCnt = getU16BE(pos + 6, &ok) / 2;
    a = -1;
    b = segCnt - 1;
    segEnd = getU16BE(pos + 14 + 2*b, &ok);
    if (c > segEnd) {
      // malformed font -- the TrueType spec requires the last segEnd
      // to be 0xffff
      return 0;
    }
    // invariant: seg[a].end < code <= seg[b].end
    while (b - a > 1 && ok) {
      m = (a + b) / 2;
      segEnd = getU16BE(pos + 14 + 2*m, &ok);
      if (segEnd < c) {
	a = m;
      } else {
	b = m;
      }
    }
    segStart = getU16BE(pos + 16 + 2*segCnt + 2*b, &ok);
    segDelta = getU16BE(pos + 16 + 4*segCnt + 2*b, &ok);
    segOffset = getU16BE(pos + 16 + 6*segCnt + 2*b, &ok);
    if (c < segStart) {
      return 0;
    }
    if (segOffset == 0) {
      gid = (c + segDelta) & 0xffff;
    } else {
      gid = getU16BE(pos + 16 + 6*segCnt + 2*b +
		       segOffset + 2 * (c - segStart), &ok);
      if (gid != 0) {
	gid = (gid + segDelta) & 0xffff;
      }
    }
    break;
  case 6:
    cmapFirst = getU16BE(pos + 6, &ok);
    cmapLen = getU16BE(pos + 8, &ok);
    if (c < cmapFirst || c >= cmapFirst + cmapLen) {
      return 0;
    }
    gid = getU16BE(pos + 10 + 2 * (c - cmapFirst), &ok);
    break;
  case 12:
    segCnt = getU32BE(pos + 12, &ok);
    a = -1;
    b = segCnt - 1;
    segEnd = getU32BE(pos + 16 + 12*b+4, &ok);
    if (c > segEnd) {
      return 0;
    }
    // invariant: seg[a].end < code <= seg[b].end
    while (b - a > 1 && ok) {
      m = (a + b) / 2;
      segEnd = getU32BE(pos + 16 + 12*m+4, &ok);
      if (segEnd < c) {
	a = m;
      } else {
	b = m;
      }
    }
    segStart = getU32BE(pos + 16 + 12*b, &ok);
    segDelta = getU32BE(pos + 16 + 12*b+8, &ok);
    if (c < segStart) {
      return 0;
    }
    gid = segDelta + (c-segStart);
    break;
  default:
    return 0;
  }
  if (!ok) {
    return 0;
  }
  return gid;
}