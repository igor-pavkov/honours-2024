char *findAttrColor(void *obj, attrsym_t *colorattr, char *dflt){
    char *color;

    if(colorattr != NULL)
      color = late_nnstring(obj, colorattr, dflt);
    else if(dflt != NULL && dflt[0])
      color = dflt;
    else
      color = DEFAULT_FILL;
    return color;
}