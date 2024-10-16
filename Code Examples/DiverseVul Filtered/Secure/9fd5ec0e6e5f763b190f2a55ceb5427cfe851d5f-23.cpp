void PDFDoc::setDocInfoModified(Object *infoObj)
{
  Object infoObjRef = getDocInfoNF();
  xref->setModifiedObject(infoObj, infoObjRef.getRef());
}