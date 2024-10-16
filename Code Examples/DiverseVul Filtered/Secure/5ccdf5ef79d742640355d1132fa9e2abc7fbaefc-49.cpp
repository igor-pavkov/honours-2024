void modbus_free(modbus_t *ctx)
{
    if (ctx == NULL)
        return;

    ctx->backend->free(ctx);
}