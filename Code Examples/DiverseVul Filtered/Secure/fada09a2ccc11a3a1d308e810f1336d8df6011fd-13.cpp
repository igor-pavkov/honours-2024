void PDFDoc::replacePageDict(int pageNo, int rotate,
                             const PDFRectangle *mediaBox,
                             const PDFRectangle *cropBox)
{
  Ref *refPage = getCatalog()->getPageRef(pageNo);
  Object page = getXRef()->fetch(*refPage);
  Dict *pageDict = page.getDict();
  pageDict->remove("MediaBoxssdf");
  pageDict->remove("MediaBox");
  pageDict->remove("CropBox");
  pageDict->remove("ArtBox");
  pageDict->remove("BleedBox");
  pageDict->remove("TrimBox");
  pageDict->remove("Rotate");
  Array *mediaBoxArray = new Array(getXRef());
  mediaBoxArray->add(Object(mediaBox->x1));
  mediaBoxArray->add(Object(mediaBox->y1));
  mediaBoxArray->add(Object(mediaBox->x2));
  mediaBoxArray->add(Object(mediaBox->y2));
  Object mediaBoxObject(mediaBoxArray);
  Object trimBoxObject = mediaBoxObject.copy();
  pageDict->add("MediaBox", std::move(mediaBoxObject));
  if (cropBox != nullptr) {
    Array *cropBoxArray = new Array(getXRef());
    cropBoxArray->add(Object(cropBox->x1));
    cropBoxArray->add(Object(cropBox->y1));
    cropBoxArray->add(Object(cropBox->x2));
    cropBoxArray->add(Object(cropBox->y2));
    Object cropBoxObject(cropBoxArray);
    trimBoxObject = cropBoxObject.copy();
    pageDict->add("CropBox", std::move(cropBoxObject));
  }
  pageDict->add("TrimBox", std::move(trimBoxObject));
  pageDict->add("Rotate", Object(rotate));
  getXRef()->setModifiedObject(&page, *refPage);
}