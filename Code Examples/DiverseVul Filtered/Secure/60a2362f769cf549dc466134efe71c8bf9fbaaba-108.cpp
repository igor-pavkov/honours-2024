void regulator_put(struct regulator *regulator)
{
	mutex_lock(&regulator_list_mutex);
	_regulator_put(regulator);
	mutex_unlock(&regulator_list_mutex);
}