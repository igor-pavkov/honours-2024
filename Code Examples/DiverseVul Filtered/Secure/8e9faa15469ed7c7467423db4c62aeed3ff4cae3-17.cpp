static unsigned int cp2112_gpio_irq_startup(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cp2112_device *dev = gpiochip_get_data(gc);

	INIT_DELAYED_WORK(&dev->gpio_poll_worker, cp2112_gpio_poll_callback);

	cp2112_gpio_direction_input(gc, d->hwirq);

	if (!dev->gpio_poll) {
		dev->gpio_poll = true;
		schedule_delayed_work(&dev->gpio_poll_worker, 0);
	}

	cp2112_gpio_irq_unmask(d);
	return 0;
}