static WSGIScriptFile *newWSGIScriptFile(apr_pool_t *p)
{
    WSGIScriptFile *object = NULL;

    object = (WSGIScriptFile *)apr_pcalloc(p, sizeof(WSGIScriptFile));

    object->handler_script = NULL;
    object->application_group = NULL;
    object->process_group = NULL;

    return object;
}