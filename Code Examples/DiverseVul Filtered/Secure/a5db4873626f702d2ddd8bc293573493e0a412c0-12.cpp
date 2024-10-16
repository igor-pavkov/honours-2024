static void SVGEndDocument(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Called when the document end has been detected.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.endDocument()");
  svg_info=(SVGInfo *) context;
  if (svg_info->offset != (char *) NULL)
    svg_info->offset=DestroyString(svg_info->offset);
  if (svg_info->stop_color != (char *) NULL)
    svg_info->stop_color=DestroyString(svg_info->stop_color);
  if (svg_info->scale != (double *) NULL)
    svg_info->scale=(double *) RelinquishMagickMemory(svg_info->scale);
  if (svg_info->text != (char *) NULL)
    svg_info->text=DestroyString(svg_info->text);
  if (svg_info->vertices != (char *) NULL)
    svg_info->vertices=DestroyString(svg_info->vertices);
  if (svg_info->url != (char *) NULL)
    svg_info->url=DestroyString(svg_info->url);
#if defined(MAGICKCORE_XML_DELEGATE)
  if (svg_info->document != (xmlDocPtr) NULL)
    {
      xmlFreeDoc(svg_info->document);
      svg_info->document=(xmlDocPtr) NULL;
    }
#endif
}