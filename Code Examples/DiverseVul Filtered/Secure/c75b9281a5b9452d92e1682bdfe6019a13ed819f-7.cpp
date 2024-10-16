static double shp_area (SHPObject *feat) {
  double area = 0.0;
  if (feat->nParts == 0) {
    area = area2d_polygon (feat->nVertices, feat->padfX, feat->padfY);
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
      area += area2d_polygon (n, &(feat->padfX[feat->panPartStart[part]]),
			      &(feat->padfY[feat->panPartStart[part]]));
    }
  }
  /* our area function computes in opposite direction */
  return -area;
}