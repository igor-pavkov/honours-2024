static inline void get_dev (struct dev_data *data)
{
	refcount_inc (&data->count);
}