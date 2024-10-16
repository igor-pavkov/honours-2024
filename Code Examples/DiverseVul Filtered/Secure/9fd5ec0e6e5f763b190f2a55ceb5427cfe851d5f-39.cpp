std::vector<FormWidgetSignature*> PDFDoc::getSignatureWidgets()
{
  int num_pages = getNumPages();
  FormPageWidgets *page_widgets = nullptr;
  std::vector<FormWidgetSignature*> widget_vector;

  for (int i = 1; i <= num_pages; i++) {
    Page *p = getCatalog()->getPage(i);
    if (p) {
      page_widgets = p->getFormWidgets();
      for (int j = 0; page_widgets != nullptr && j < page_widgets->getNumWidgets(); j++) {
	if (page_widgets->getWidget(j)->getType() == formSignature) {
	    widget_vector.push_back(static_cast<FormWidgetSignature*>(page_widgets->getWidget(j)));
	}
      }
      delete page_widgets;
    }
  }
  return widget_vector;
}