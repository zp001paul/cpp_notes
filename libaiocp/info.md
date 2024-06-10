# ToDo

1. log_warning OK
1. log_info OK
1. copy file attr
1. adaptive iodepth_submit / RWF_NOWAIT

# BUGS:

1. BUG: fix sharding size align OK
1. 还是会出现以下日志，不是哪里计数错了 OK
   libaiocp : warn: tid:0, targeting iocb slot 0 not ready, status: 3, try to reap!
   libaiocp : error: tid:0, no write io in queue! exiting!
1. io线程提前退出，主线程没能退出 OK
1. 完全顺利跑完，数据也不对

# feature

1. copy big file with multiple threads
2. customized AIO iodepth as FIO did
3. support AIO userspace reap
4. support direct IO mode and buffered IO mode

# algorithem

全局数据：

1. io_r_cnt_in_q
1. io_w_cnt_in_q

局部数据：
对齐数据：

1. r_cbs
2. ctx
   不对齐数据：
3. w_cbs

算法：

1. 挨个io prepair
1. prepair cnt > iodepth_submit, 则 io_submit, io_r_cnt_in_q += submit-cnt
1. io\*submit之后(如果有submit过IO)io_getevent(min=iodepth_complete_min, max=io_r_cnt_in_q+io_w_cnt_in_q)
   1. 如果是读：
      1. 把ret个io标记为读完成
      1. 提交ret个写io
   2. 如果是写：
      1. 把ret个io标记为写完成
