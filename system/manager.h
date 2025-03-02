#pragma once

#include "helper.h"
#include "global.h"

class row_t;
class TxnManager;
class workload;
class Transport;

// Global Manager shared by all the threads.
// For distributed version, Manager is shared by all threads on a single node.
class Manager {
public:
	void 			init();

	// Global timestamp allocation
	uint64_t		get_ts(uint64_t thread_id);

	// For MVCC. To calculate the min active ts in the system
	void 			add_ts(uint64_t ts);
	uint64_t 		get_min_ts(uint64_t tid = 0);

	// For DL_DETECT.
	void 			set_txn_man(TxnManager * txn);
	TxnManager * 	get_txn_man(int thd_id) { return _all_txns[thd_id]; };

	// per-thread random number generator
	void 			init_rand(uint64_t thd_id) {  srand48_r(thd_id, &_buffer); }
	uint64_t		rand_uint64();
	uint64_t		rand_uint64(uint64_t max);
	uint64_t		rand_uint64(uint64_t min, uint64_t max);
	double 			rand_double();

	// thread id
	void 			set_thd_id(uint64_t thread_id) { _thread_id = thread_id; }
	uint64_t		get_thd_id() { return _thread_id; }

	// TICTOC, max_cts
	void 			set_max_cts(uint64_t cts) { _max_cts = cts; }
	uint64_t 		get_max_cts() { return _max_cts; }

	// workload
	void 			set_workload(workload * wl)	{ _workload = wl; }
	inline workload *		get_workload()	{ return _workload; }

	// For client server thd id conversion
	uint32_t 		txnid_to_server_node(uint64_t txn_id);
	uint32_t 		txnid_to_server_thread(uint64_t txn_id);

	// global synchronization
	bool is_sim_done();
	void set_remote_done() { _remote_done = true; }
	uint32_t worker_thread_done();
	void remote_node_done();
	bool are_all_worker_threads_done() { return _num_finished_worker_threads == g_num_worker_threads; }

	// TCM global timer
	uint64_t 		get_current_time();
	uint64_t 		get_gc_ts() { return _global_gc_min_ts; }
	void 	 		set_gc_ts(uint64_t ts);
	void 			update_global_gc_ts(uint32_t node_id, uint64_t ts);
private:
	pthread_mutex_t ts_mutex;
	uint64_t *		timestamp;
	pthread_mutex_t mutexes[BUCKET_CNT];
	uint64_t 		hash(row_t * row);
	uint64_t volatile * volatile * volatile all_ts;
	TxnManager ** 	_all_txns;

	bool volatile 	_remote_done;
	uint32_t 		_num_finished_worker_threads;
	uint32_t 		_num_finished_remote_nodes;

	// per-thread random number
	static __thread drand48_data _buffer;

	// thread id
	static __thread uint64_t _thread_id;

	// For TICTOC timestamp
	static __thread uint64_t _max_cts; // max commit timestamp seen by the thread so far.

	// thread local transport
	//static __thread Transport * _transport;

	// workload
	workload * _workload;

	// for MVCC
	volatile uint64_t	_last_min_ts_time;
	uint64_t			_min_ts;

	// TCM global timer.
	ALIGNED(64) uint64_t 			_global_gc_min_ts;
	ALIGNED(64) uint64_t 	_timestamp_counter;
	// For TCM GC. Maintain the lowest early timestamp ever seen
	uint64_t **		_early_per_thread;
	uint64_t **     _gc_ts_per_node;
};
