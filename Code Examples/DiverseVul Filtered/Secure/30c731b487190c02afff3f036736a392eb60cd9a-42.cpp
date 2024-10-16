void HtmlPage::addChar(GfxState *state, double x, double y,
		       double dx, double dy, 
			double ox, double oy, Unicode *u, int uLen) {
  double x1, y1, w1, h1, dx2, dy2;
  int n, i;
  state->transform(x, y, &x1, &y1);
  n = curStr->len;
 
  // check that new character is in the same direction as current string
  // and is not too far away from it before adding 
  //if ((UnicodeMap::getDirection(u[0]) != curStr->dir) || 
  // XXX
  if (debug) {
    const double *text_mat = state->getTextMat();
    // rotation is (cos q, sin q, -sin q, cos q, 0, 0)
    // sin q is zero iff there is no rotation, or 180 deg. rotation;
    // for 180 rotation, cos q will be negative
    if (text_mat[0] < 0 || !is_within(text_mat[1], .1, 0)) {
      std::cerr << DEBUG << "rotation matrix for \"" << print_uni_str(u, uLen) << '"' << std::endl;
      std::cerr << "text " << print_matrix(state->getTextMat());
    }
  }
  if (n > 0 && // don't start a new string, unless there is already a string
      // TODO: the following line assumes that text is flowing left to
      // right, which will not necessarily be the case, e.g. if rotated;
      // It assesses whether or not two characters are close enough to
      // be part of the same string
      fabs(x1 - curStr->xRight[n-1]) > wordBreakThreshold * (curStr->yMax - curStr->yMin) &&
      // rotation is (cos q, sin q, -sin q, cos q, 0, 0)
      // sin q is zero iff there is no rotation, or 180 deg. rotation;
      // for 180 rotation, cos q will be negative
      !rot_matrices_equal(curStr->getFont().getRotMat(), state->getTextMat()))
  {
    endString();
    beginString(state, nullptr);
  }
  state->textTransformDelta(state->getCharSpace() * state->getHorizScaling(),
			    0, &dx2, &dy2);
  dx -= dx2;
  dy -= dy2;
  state->transformDelta(dx, dy, &w1, &h1);
  if (uLen != 0) {
    w1 /= uLen;
    h1 /= uLen;
  }
  for (i = 0; i < uLen; ++i) {
    curStr->addChar(state, x1 + i*w1, y1 + i*h1, w1, h1, u[i]);
  }
}