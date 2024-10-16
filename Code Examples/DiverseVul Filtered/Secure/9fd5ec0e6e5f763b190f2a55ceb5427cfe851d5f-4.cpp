int PDFDoc::saveAs(OutStream *outStr, PDFWriteMode mode) {
  if (file && file->modificationTimeChangedSinceOpen())
    return errFileChangedSinceOpen;

  if (!xref->isModified() && mode == writeStandard) {
    // simply copy the original file
    saveWithoutChangesAs (outStr);
  } else if (mode == writeForceRewrite) {
    saveCompleteRewrite(outStr);
  } else {
    saveIncrementalUpdate(outStr);
  }

  return errNone;
}