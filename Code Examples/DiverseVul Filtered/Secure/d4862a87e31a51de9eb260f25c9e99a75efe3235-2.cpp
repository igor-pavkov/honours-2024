static void pit_realizefn(DeviceState *dev, Error **errp)
{
    PITCommonState *pit = PIT_COMMON(dev);
    PITClass *pc = PIT_GET_CLASS(dev);
    PITChannelState *s;

    s = &pit->channels[0];
    /* the timer 0 is connected to an IRQ */
    s->irq_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, pit_irq_timer, s);
    qdev_init_gpio_out(dev, &s->irq, 1);

    memory_region_init_io(&pit->ioports, OBJECT(pit), &pit_ioport_ops,
                          pit, "pit", 4);

    qdev_init_gpio_in(dev, pit_irq_control, 1);

    pc->parent_realize(dev, errp);
}