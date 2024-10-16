static void CloseTags( GooString *htext, bool &finish_a, bool &finish_italic, bool &finish_bold )
{
  const char *last_italic = finish_italic && ( finish_bold   || finish_a    ) ? strrstr( htext->c_str(), "<i>" ) : nullptr;
  const char *last_bold   = finish_bold   && ( finish_italic || finish_a    ) ? strrstr( htext->c_str(), "<b>" ) : nullptr;
  const char *last_a      = finish_a      && ( finish_italic || finish_bold ) ? strrstr( htext->c_str(), "<a " ) : nullptr;
  if( finish_a && ( finish_italic || finish_bold ) && last_a > ( last_italic > last_bold ? last_italic : last_bold ) ){
    htext->append("</a>", 4);
    finish_a = false;
  }
  if( finish_italic && finish_bold && last_italic > last_bold ){
    htext->append("</i>", 4);
    finish_italic = false;
  }
  if( finish_bold )
    htext->append("</b>", 4);
  if( finish_italic )
    htext->append("</i>", 4);
  if( finish_a )
    htext->append("</a>");
}