struct regulator *regulator_get(struct device *dev, const char *id)
{
	return _regulator_get(dev, id, false, true);
}