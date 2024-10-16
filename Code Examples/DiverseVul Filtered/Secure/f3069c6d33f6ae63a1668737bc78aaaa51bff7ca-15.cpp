void __rds_put_mr_final(struct rds_mr *mr)
{
	rds_destroy_mr(mr);
	kfree(mr);
}