static void __attribute__((destructor)) _nm_singleton_instance_destroy(void)
{
    _singletons_shutdown = TRUE;

    while (_singletons) {
        GObject *instance = _singletons->data;

        _singletons = g_slist_delete_link(_singletons, _singletons);

        g_object_weak_unref(instance, _nm_singleton_instance_weak_cb, NULL);

        if (instance->ref_count > 1) {
            nm_log_dbg(LOGD_CORE,
                       "disown %s singleton (" NM_HASH_OBFUSCATE_PTR_FMT ")",
                       G_OBJECT_TYPE_NAME(instance),
                       NM_HASH_OBFUSCATE_PTR(instance));
        }

        g_object_unref(instance);
    }
}