void HtmlPage::conv(){
  for(HtmlString *tmp=yxStrings;tmp;tmp=tmp->yxNext){
     delete tmp->htext;
     tmp->htext=HtmlFont::HtmlFilter(tmp->text,tmp->len);

     int linkIndex = 0;
     if (links->inLink(tmp->xMin,tmp->yMin,tmp->xMax,tmp->yMax, linkIndex)){
       tmp->link = links->getLink(linkIndex);
     }
  }
}