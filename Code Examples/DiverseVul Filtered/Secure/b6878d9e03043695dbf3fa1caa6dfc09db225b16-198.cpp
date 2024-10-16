void md_kick_rdev_from_array(struct md_rdev *rdev)
{
	unbind_rdev_from_array(rdev);
	export_rdev(rdev);
}