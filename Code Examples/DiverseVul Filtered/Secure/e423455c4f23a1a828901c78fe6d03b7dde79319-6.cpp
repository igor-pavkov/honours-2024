static void fw_cfg_resized(const char *id, uint64_t length, void *host)
{
    if (fw_cfg) {
        fw_cfg_modify_file(fw_cfg, id + strlen("/rom@"), host, length);
    }
}