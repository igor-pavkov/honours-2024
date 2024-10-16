bool PDFDoc::checkEncryption(const GooString *ownerPassword, const GooString *userPassword) {
  bool encrypted;
  bool ret;

  Object encrypt = xref->getTrailerDict()->dictLookup("Encrypt");
  if ((encrypted = encrypt.isDict())) {
    if ((secHdlr = SecurityHandler::make(this, &encrypt))) {
      if (secHdlr->isUnencrypted()) {
	// no encryption
	ret = true;
      } else if (secHdlr->checkEncryption(ownerPassword, userPassword)) {
	// authorization succeeded
       	xref->setEncryption(secHdlr->getPermissionFlags(),
			    secHdlr->getOwnerPasswordOk(),
			    secHdlr->getFileKey(),
			    secHdlr->getFileKeyLength(),
			    secHdlr->getEncVersion(),
			    secHdlr->getEncRevision(),
			    secHdlr->getEncAlgorithm());
	ret = true;
      } else {
	// authorization failed
	ret = false;
      }
    } else {
      // couldn't find the matching security handler
      ret = false;
    }
  } else {
    // document is not encrypted
    ret = true;
  }
  return ret;
}