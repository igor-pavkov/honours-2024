void modbus_close(modbus_t *ctx)
{
    if (ctx == NULL)
        return;

    ctx->backend->close(ctx);
}