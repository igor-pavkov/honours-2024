static WSGIDirectoryConfig *newWSGIDirectoryConfig(apr_pool_t *p)
{
    WSGIDirectoryConfig *object = NULL;

    object = (WSGIDirectoryConfig *)apr_pcalloc(p, sizeof(WSGIDirectoryConfig));

    object->pool = p;

    object->process_group = NULL;
    object->application_group = NULL;
    object->callable_object = NULL;

    object->dispatch_script = NULL;

    object->pass_apache_request = -1;
    object->pass_authorization = -1;
    object->script_reloading = -1;
    object->error_override = -1;
    object->chunked_request = -1;

    object->enable_sendfile = -1;

    object->access_script = NULL;
    object->auth_user_script = NULL;
    object->auth_group_script = NULL;
    object->user_authoritative = -1;
    object->group_authoritative = -1;

    return object;
}