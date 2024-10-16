void PDFDoc::displayPageSlice(OutputDev *out, int page,
			      double hDPI, double vDPI, int rotate,
			      bool useMediaBox, bool crop, bool printing,
			      int sliceX, int sliceY, int sliceW, int sliceH,
			      bool (*abortCheckCbk)(void *data),
			      void *abortCheckCbkData,
                              bool (*annotDisplayDecideCbk)(Annot *annot, void *user_data),
                              void *annotDisplayDecideCbkData, bool copyXRef) {
  if (getPage(page))
    getPage(page)->displaySlice(out, hDPI, vDPI,
					 rotate, useMediaBox, crop,
					 sliceX, sliceY, sliceW, sliceH,
					 printing,
					 abortCheckCbk, abortCheckCbkData,
					 annotDisplayDecideCbk, annotDisplayDecideCbkData, copyXRef);
}