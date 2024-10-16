Linearization *PDFDoc::getLinearization()
{
  if (!linearization) {
    linearization = new Linearization(str);
    linearizationState = 0;
  }
  return linearization;
}