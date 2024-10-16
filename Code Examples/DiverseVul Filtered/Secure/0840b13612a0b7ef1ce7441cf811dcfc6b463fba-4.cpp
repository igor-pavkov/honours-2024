void msre_engine_reqbody_processor_register(msre_engine *engine,
    const char *name, void *fn_init, void *fn_process, void *fn_complete)
{
    msre_reqbody_processor_metadata *metadata = 
        (msre_reqbody_processor_metadata *)apr_pcalloc(engine->mp,
            sizeof(msre_reqbody_processor_metadata));
    if (metadata == NULL) return;

    metadata->name = name;
    metadata->init = fn_init;
    metadata->process = fn_process;
    metadata->complete = fn_complete;
    apr_table_setn(engine->reqbody_processors, name, (void *)metadata);
}