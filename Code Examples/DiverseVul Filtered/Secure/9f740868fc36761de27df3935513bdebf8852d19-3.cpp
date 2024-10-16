void SWTPM_NVRAM_Shutdown(void)
{
    if (g_nvram_backend_ops)
        g_nvram_backend_ops->cleanup();
    memset(&filekey, 0, sizeof(filekey));
    memset(&migrationkey, 0, sizeof(migrationkey));
}