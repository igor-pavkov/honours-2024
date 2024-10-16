bool PDFDoc::isLinearized(bool tryingToReconstruct) {
  if ((str->getLength()) &&
      (getLinearization()->getLength() == str->getLength()))
    return true;
  else {
    if (tryingToReconstruct)
      return getLinearization()->getLength() > 0;
    else
      return false;
  }
}