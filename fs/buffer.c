struct buffer_head * bread(int dev, int block){
/*	struct buffer_head * bh;
	if (!(bh = getblk(dev,block)))
		panic("");
	if (bh->b_uptodata)
		return bh;
	ll_rw_block(READ,bh);
	wait_on_buffer(bh);
	if(bh->b_uptodata)
		return bh;
	brelse(bh);
*/	return ((void *)0);
}
