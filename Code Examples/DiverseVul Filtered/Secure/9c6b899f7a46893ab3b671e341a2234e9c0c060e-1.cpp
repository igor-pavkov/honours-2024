static void local_cleanup(FsContext *ctx)
{
    LocalData *data = ctx->private;

    close(data->mountfd);
    g_free(data);
}