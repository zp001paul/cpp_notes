# 好文章

https://blog.csdn.net/majianting/article/details/105770083 # 网友的例子
https://blog.csdn.net/qq_40989769/article/details/129104963 # aio和epoll的结合
https://lwn.net/Articles/508064/ # 官文
https://blog.csdn.net/yiyeguzhou100/article/details/106289024 # 内核源码调用路径
https://lwn.net/Articles/724198/ # 讲AIO和O_DIRECT的关系
https://lwn.net/Kernel/Index/#Asynchronous_IO # lwn上面关于AIO的文章列表

# AIO的各种使用模式

open模式

1. 正常direct IO模式
2. 异步fsync()

prep模式

1. 正常模式
2. nowait

reap模式

1. 正常io_getevents()
2. user reap
3. epoll结合

## direct IO在哪里判断是否要等待？

```txt
__blkdev_direct_IO()
    ->is_sync_kiocb() // 判断是否在aio上下文
```

逻辑真是很简单

```c

static inline bool is_sync_kiocb(struct kiocb *kiocb)
{
	return kiocb->ki_complete == NULL;
}

```
