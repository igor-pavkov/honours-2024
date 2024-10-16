struct regmap *regulator_get_regmap(struct regulator *regulator)
{
	struct regmap *map = regulator->rdev->regmap;

	return map ? map : ERR_PTR(-EOPNOTSUPP);
}