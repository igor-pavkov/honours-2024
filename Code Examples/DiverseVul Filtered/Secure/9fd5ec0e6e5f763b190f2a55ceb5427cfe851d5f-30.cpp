void PDFDoc::displayPage(OutputDev *out, int page,
			 double hDPI, double vDPI, int rotate,
			 bool useMediaBox, bool crop, bool printing,
			 bool (*abortCheckCbk)(void *data),
			 void *abortCheckCbkData,
                         bool (*annotDisplayDecideCbk)(Annot *annot, void *user_data),
                         void *annotDisplayDecideCbkData, bool copyXRef) {
  if (globalParams->getPrintCommands()) {
    printf("***** page %d *****\n", page);
  }

  if (getPage(page))
    getPage(page)->display(out, hDPI, vDPI,
				    rotate, useMediaBox, crop, printing,
				    abortCheckCbk, abortCheckCbkData,
				    annotDisplayDecideCbk, annotDisplayDecideCbkData, copyXRef);

}