void rom_set_order_override(int order)
{
    if (!fw_cfg)
        return;
    fw_cfg_set_order_override(fw_cfg, order);
}