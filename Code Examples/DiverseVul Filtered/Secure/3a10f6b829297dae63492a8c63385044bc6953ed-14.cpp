char *msPostGISBuildSQLFrom(layerObj *layer, rectObj *rect)
{
  char *strFrom = 0;
  msPostGISLayerInfo *layerinfo;

  if (layer->debug) {
    msDebug("msPostGISBuildSQLFrom called.\n");
  }

  assert( layer->layerinfo != NULL);

  layerinfo = (msPostGISLayerInfo *)layer->layerinfo;

  if ( ! layerinfo->fromsource ) {
    msSetError(MS_MISCERR, "Layerinfo->fromsource is not initialized.", "msPostGISBuildSQLFrom()");
    return NULL;
  }

  /*
  ** If there's a '!BOX!' in our source we need to substitute the
  ** current rectangle for it...
  */
  strFrom = msPostGISReplaceBoxToken(layer, rect, layerinfo->fromsource);

  return strFrom;
}