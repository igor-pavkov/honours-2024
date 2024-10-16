cmsBool RegisterTypesPlugin(cmsContext id, cmsPluginBase* Data, _cmsMemoryClient pos)
{
    cmsPluginTagType* Plugin = (cmsPluginTagType*) Data;
    _cmsTagTypePluginChunkType* ctx = ( _cmsTagTypePluginChunkType*) _cmsContextGetClientChunk(id, pos);
    _cmsTagTypeLinkedList *pt;

    // Calling the function with NULL as plug-in would unregister the plug in.
    if (Data == NULL) {

        // There is no need to set free the memory, as pool is destroyed as a whole.
        ctx ->TagTypes = NULL;
        return TRUE;
    }

    // Registering happens in plug-in memory pool.
    pt = (_cmsTagTypeLinkedList*) _cmsPluginMalloc(id, sizeof(_cmsTagTypeLinkedList));
    if (pt == NULL) return FALSE;

    pt ->Handler   = Plugin ->Handler;
    pt ->Next      = ctx ->TagTypes;

    ctx ->TagTypes = pt;
     
    return TRUE;
}