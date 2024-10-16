static inline void em28xx_set_model(struct em28xx *dev)
{
	dev->board = em28xx_boards[dev->model];
	dev->has_msp34xx = dev->board.has_msp34xx;
	dev->is_webcam = dev->board.is_webcam;

	em28xx_set_xclk_i2c_speed(dev);

	/* Should be initialized early, for I2C to work */
	dev->def_i2c_bus = dev->board.def_i2c_bus;
}