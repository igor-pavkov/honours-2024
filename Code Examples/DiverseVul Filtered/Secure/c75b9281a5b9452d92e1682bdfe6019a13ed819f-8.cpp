static double shp_length (SHPObject *feat) {
  double length = 0.0;
  if (feat->nParts == 0) {
    length = length2d_polyline(feat->nVertices, feat->padfX, feat->padfY);
  }
  else {
    for (int part = 0; part < feat->nParts; part++) {
      int n;
      if (part < feat->nParts - 1) {
	n = feat->panPartStart[part+1] - feat->panPartStart[part];
      }
      else {
	n = feat->nVertices - feat->panPartStart[part];
      }
      length += length2d_polyline (n,
				   &(feat->padfX[feat->panPartStart[part]]),
				   &(feat->padfY[feat->panPartStart[part]]));
    }
  }
  return length;
}