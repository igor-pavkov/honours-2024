static void security_info_free(struct security_info *i) {
        if (!i)
                return;

        free(i->id);
        free(i->type);
        free(i->load_state);
        free(i->fragment_path);

        free(i->user);

        free(i->protect_home);
        free(i->protect_system);

        free(i->root_directory);
        free(i->root_image);

        free(i->keyring_mode);
        free(i->notify_access);

        free(i->device_policy);

        strv_free(i->supplementary_groups);
        strv_free(i->system_call_architectures);

        set_free_free(i->system_call_filter);
}