/*	COP 4610 U01
	Michael Garrett
	Winicius Siqueira
	Michael Weiland
 The only method different in this file that differs from those in block/clook-iosched.c is noop_add_request (called "clook_add_request" in this file), which has been modified to use a C-LOOK queueing algorithm instead of a FIFO queue. The algorithm sorts the requests according to their end sectors in ascending order. Every time a new request is to be added, the algorithm iterates through the queue, starting at the beginning, until it either finds a request with a greater end sector or reaches the end of the queue, and then loads the current request into that position.

In addition, printk() statements were added in clook_dispatch and clook_add_request to show, whenever a request was added to the dispatch queue or scheduler list, the sector number of the request as well as whether it was a read or write operation. It should be mentioned that the following were used in printing these statements:
* rq_data_dir: a preproccesor macro found in include/linux/blkdev.h that returns the type of a request (read or write).
* REQ_WRITE: a preproccesor macro found in include/linux/blk_types.h that redirects the type of a request (read or write) to standard output.
* blk_rq_pos(): a function found in include/linux/blkdev.h that returns the current sector of a request.
*/

/*
 * elevator clook
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct clook_data {
	struct list_head queue;
};

static void clook_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int clook_dispatch(struct request_queue *q, int force)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		//new
		printk("[CLOOK] dsp %c %lu\n",(rq_data_dir(rq) & REQ_WRITE) ? 'W' : 'R',blk_rq_pos(rq));
		return 1;
	}
	return 0;
}

static void clook_add_request(struct request_queue *q, struct request *rq)
{
	//old
	/*struct clook_data *nd = q->elevator->elevator_data;

	list_add_tail(&rq->queuelist, &nd->queue);*/

	//new
	struct clook_data *cd = q->elevator->elevator_data; //cd stands for "CLOOK data"
	struct list_head *p = NULL; //pointer to traverse queue
	list_for_each(p, &cd->queue) //traverse the queue
/*rq_end_sector is a macro defined at the end of elevator.h which computes the end sector of a
request by adding its current sector to the number of sectors left to traverse in the
request. This queue algorithm sorts the requests by their end sectors.*/
		if(rq_end_sector(list_entry(p,struct request,queuelist)) >= rq_end_sector(rq))
			break;
	list_add_tail(&rq->queuelist, p);
	printk("[CLOOK] add %c %lu\n",(rq_data_dir(rq) & REQ_WRITE) ? 'W' : 'R',blk_rq_pos(rq));
}

static int clook_queue_empty(struct request_queue *q)
{
	struct clook_data *nd = q->elevator->elevator_data;

	return list_empty(&nd->queue);
}

static struct request *
clook_former_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
clook_latter_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static void *clook_init_queue(struct request_queue *q)
{
	struct clook_data *nd;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return NULL;
	INIT_LIST_HEAD(&nd->queue);
	return nd;
}

static void clook_exit_queue(struct elevator_queue *e)
{
	struct clook_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_clook = {
	.ops = {
		.elevator_merge_req_fn		= clook_merged_requests,
		.elevator_dispatch_fn		= clook_dispatch,
		.elevator_add_req_fn		= clook_add_request,
		.elevator_queue_empty_fn	= clook_queue_empty,
		.elevator_former_req_fn		= clook_former_request,
		.elevator_latter_req_fn		= clook_latter_request,
		.elevator_init_fn		= clook_init_queue,
		.elevator_exit_fn		= clook_exit_queue,
	},
	.elevator_name = "clook",
	.elevator_owner = THIS_MODULE,
};

static int __init clook_init(void)
{
	elv_register(&elevator_clook);

	return 0;
}

static void __exit clook_exit(void)
{
	elv_unregister(&elevator_clook);
}

module_init(clook_init);
module_exit(clook_exit);

/* old
MODULE_AUTHOR("Jens Axboe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("No-op IO scheduler");*/

//new
MODULE_AUTHOR("Michael Garrett, Winicius Siqueira, Michael Weiland");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("C-Look IO scheduler");
