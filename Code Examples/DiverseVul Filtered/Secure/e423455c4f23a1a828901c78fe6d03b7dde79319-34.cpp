void rom_reset_order_override(void)
{
    if (!fw_cfg)
        return;
    fw_cfg_reset_order_override(fw_cfg);
}