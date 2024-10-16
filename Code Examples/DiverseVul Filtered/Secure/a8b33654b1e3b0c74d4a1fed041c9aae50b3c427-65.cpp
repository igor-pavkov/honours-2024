void mp_unregister_driver(struct uart_driver *drv)
{
    struct tty_driver *normal = NULL;

    normal = drv->tty_driver;

    if (!normal)
    {
        return;
    }

    tty_unregister_driver(normal);
    put_tty_driver(normal);
    drv->tty_driver = NULL;


    if (drv->state)
    {
        kfree(drv->state);
    }

}