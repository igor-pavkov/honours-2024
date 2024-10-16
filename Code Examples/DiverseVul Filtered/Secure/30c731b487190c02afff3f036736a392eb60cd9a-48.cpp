HtmlOutputDev::~HtmlOutputDev() {
    delete Docname;
    delete docTitle;

    for (auto entry : *glMetaVars) {
      delete entry;
    }
    delete glMetaVars;

    if (fContentsFrame){
      fputs("</body>\n</html>\n",fContentsFrame);  
      fclose(fContentsFrame);
    }
    if (page != nullptr) {
      if (xml) {
        fputs("</pdf2xml>\n",page);  
        fclose(page);
      } else
      if ( !complexMode || xml || noframes )
      { 
        fputs("</body>\n</html>\n",page);  
        fclose(page);
      }
    }
    if (pages)
      delete pages;
}