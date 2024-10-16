void msPostGISLayerFreeItemInfo(layerObj *layer)
{
#ifdef USE_POSTGIS
  if (layer->debug) {
    msDebug("msPostGISLayerFreeItemInfo called.\n");
  }

  if (layer->iteminfo) {
    free(layer->iteminfo);
  }
  layer->iteminfo = NULL;
#endif
}