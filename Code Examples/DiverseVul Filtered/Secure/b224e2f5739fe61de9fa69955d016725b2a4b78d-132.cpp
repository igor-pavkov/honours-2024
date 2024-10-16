SplashPath SplashOutputDev::convertPath(GfxState *state, GfxPath *path,
					 bool dropEmptySubpaths) {
  SplashPath sPath;
  GfxSubpath *subpath;
  int n, i, j;

  n = dropEmptySubpaths ? 1 : 0;
  for (i = 0; i < path->getNumSubpaths(); ++i) {
    subpath = path->getSubpath(i);
    if (subpath->getNumPoints() > n) {
      sPath.reserve(subpath->getNumPoints() + 1);
      sPath.moveTo((SplashCoord)subpath->getX(0),
		    (SplashCoord)subpath->getY(0));
      j = 1;
      while (j < subpath->getNumPoints()) {
	if (subpath->getCurve(j)) {
	  sPath.curveTo((SplashCoord)subpath->getX(j),
			 (SplashCoord)subpath->getY(j),
			 (SplashCoord)subpath->getX(j+1),
			 (SplashCoord)subpath->getY(j+1),
			 (SplashCoord)subpath->getX(j+2),
			 (SplashCoord)subpath->getY(j+2));
	  j += 3;
	} else {
	  sPath.lineTo((SplashCoord)subpath->getX(j),
			(SplashCoord)subpath->getY(j));
	  ++j;
	}
      }
      if (subpath->isClosed()) {
	sPath.close();
      }
    }
  }
  return sPath;
}