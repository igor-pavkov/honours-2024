struct regulator *regulator_get_optional(struct device *dev, const char *id)
{
	return _regulator_get(dev, id, false, false);
}