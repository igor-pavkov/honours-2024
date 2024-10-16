Goffset PDFDoc::getMainXRefEntriesOffset(bool tryingToReconstruct)
{
  unsigned int mainXRefEntriesOffset = 0;

  if (isLinearized(tryingToReconstruct)) {
    mainXRefEntriesOffset = getLinearization()->getMainXRefEntriesOffset();
  }

  return mainXRefEntriesOffset;
}