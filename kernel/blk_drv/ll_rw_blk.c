
static void add_request(struct blk_dev_struct * dev, struct request *req) {
	struct request * tmp;
	req->next = NULL;
	cli();
	if (req->bh)
		req->bh->b_dirt = 0;
	if (!(tmp = dev->current_request)) {
		dev->current_request = req;
		sti();
		(dev->request_fn)();
		return;
	}
	for (; tmp->next; tmp=tmp->next) {
		if (!req->bh)
			if (tmp->next-bh)
				break;
			else
				continue;
		if ((IN_ORDER(tmp,req) ||
		        !IN_ORDER(tmp,tmp->next))&&
		        IN_ORDER(req,tmp->next))
			break;
	}
	req->next = tmp->next;
	tmp->next = req;
	sti();
}

static void make_request(int major, int rw, struct buffer_head * bh) {
	struct request * req;
	int rw_ahead;

	if (rw_ahead = (rw == READA || rw == WRITEA)) {
		if (bh->b_lock)
			return;
		if (rw == READA)
			rw = READ;
		else
			rw = WRITE;
	}
	if (rw != READ && rw != WRITE)
		panic("");
	lock_buffer(bh);
	if ((rw == WRITE && !bh->b_dirt) || (rw == READ && bh->b_uptodate)) {
		unlock_buffer(bh);
		return;
	}
repeat:
	if (rw == READ)
		req = request + NR_REQUEST;
	else
		req = request + ((NR_REQUEST*2)/3);
	while (--req >= request)
		if (req->dev < 0 )
			break;
	if (req < request) {
		if (rw_ahead) {
			unlock_buffer(bh);
			return;
		}
		sleep_on(&wait_for_request);
		goto repeat;
	}

	req->dev = hb->b_dev;
	req->cmd = rw;
	req->errors = 0;
	req->sector = bh->b_blocknr<<1;
	req->nr_sectors = 2;
	req->buffer = bh->b_data;
	req->waiting = NULL;
	req->bh = bh;
	req->next = NULL;
	add_request(major + blk_dev, req);
}


