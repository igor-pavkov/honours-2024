void HtmlPage::dumpAsXML(FILE* f,int page){  
  fprintf(f, "<page number=\"%d\" position=\"absolute\"", page);
  fprintf(f," top=\"0\" left=\"0\" height=\"%d\" width=\"%d\">\n", pageHeight,pageWidth);
    
  for(int i=fontsPageMarker;i < fonts->size();i++) {
    GooString *fontCSStyle = fonts->CSStyle(i);
    fprintf(f,"\t%s\n",fontCSStyle->c_str());
    delete fontCSStyle;
  }

  for (auto ptr : *imgList) {
    auto img = static_cast<HtmlImage *>(ptr);
    if (!noRoundedCoordinates) {
      fprintf(f, "<image top=\"%d\" left=\"%d\" ", xoutRound(img->yMin), xoutRound(img->xMin));
      fprintf(f, "width=\"%d\" height=\"%d\" ", xoutRound(img->xMax - img->xMin), xoutRound(img->yMax - img->yMin));
    }
    else {
      fprintf(f, "<image top=\"%f\" left=\"%f\" ", img->yMin, img->xMin);
      fprintf(f, "width=\"%f\" height=\"%f\" ", img->xMax - img->xMin, img->yMax - img->yMin);
    }
    fprintf(f,"src=\"%s\"/>\n",img->fName->c_str());
    delete img;
  }
  imgList->clear();

  for(HtmlString *tmp=yxStrings;tmp;tmp=tmp->yxNext){
    if (tmp->htext){
      if (!noRoundedCoordinates) {
        fprintf(f, "<text top=\"%d\" left=\"%d\" ", xoutRound(tmp->yMin), xoutRound(tmp->xMin));
        fprintf(f, "width=\"%d\" height=\"%d\" ", xoutRound(tmp->xMax - tmp->xMin), xoutRound(tmp->yMax - tmp->yMin));
      }
      else {
        fprintf(f, "<text top=\"%f\" left=\"%f\" ", tmp->yMin, tmp->xMin);
        fprintf(f, "width=\"%f\" height=\"%f\" ", tmp->xMax - tmp->xMin, tmp->yMax - tmp->yMin);
      }
      fprintf(f,"font=\"%d\">", tmp->fontpos);
      fputs(tmp->htext->c_str(),f);
      fputs("</text>\n",f);
    }
  }
  fputs("</page>\n",f);
}