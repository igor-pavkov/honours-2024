static void rss_data_to_rss_config(struct VirtioNetRssData *data,
                                   struct EBPFRSSConfig *config)
{
    config->redirect = data->redirect;
    config->populate_hash = data->populate_hash;
    config->hash_types = data->hash_types;
    config->indirections_len = data->indirections_len;
    config->default_queue = data->default_queue;
}