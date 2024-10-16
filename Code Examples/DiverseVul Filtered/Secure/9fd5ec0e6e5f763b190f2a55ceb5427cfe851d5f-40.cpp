void PDFDoc::displayPages(OutputDev *out, int firstPage, int lastPage,
			  double hDPI, double vDPI, int rotate,
			  bool useMediaBox, bool crop, bool printing,
			  bool (*abortCheckCbk)(void *data),
			  void *abortCheckCbkData,
                          bool (*annotDisplayDecideCbk)(Annot *annot, void *user_data),
                          void *annotDisplayDecideCbkData) {
  int page;

  for (page = firstPage; page <= lastPage; ++page) {
    displayPage(out, page, hDPI, vDPI, rotate, useMediaBox, crop, printing,
		abortCheckCbk, abortCheckCbkData,
                annotDisplayDecideCbk, annotDisplayDecideCbkData);
  }
}