bool PDFDoc::getID(GooString *permanent_id, GooString *update_id) const {
  Object obj = xref->getTrailerDict()->dictLookup ("ID");

  if (obj.isArray() && obj.arrayGetLength() == 2) {
    if (permanent_id) {
      Object obj2 = obj.arrayGet(0);
      if (obj2.isString()) {
        if (!get_id (obj2.getString(), permanent_id)) {
	  return false;
	}
      } else {
        error(errSyntaxError, -1, "Invalid permanent ID");
	return false;
      }
    }

    if (update_id) {
      Object obj2 = obj.arrayGet(1);
      if (obj2.isString()) {
        if (!get_id (obj2.getString(), update_id)) {
	  return false;
	}
      } else {
        error(errSyntaxError, -1, "Invalid update ID");
	return false;
      }
    }

    return true;
  }

  return false;
}