# 好文章

libaio man page
https://blog.csdn.net/majianting/article/details/105770083 # 网友的例子
https://blog.csdn.net/qq_40989769/article/details/129104963 # aio和epoll的结合
https://lwn.net/Articles/508064/ # 官文
https://lwn.net/Kernel/Index/#Asynchronous_IO # lwn上面关于AIO的文章列表
https://lwn.net/Articles/724198/ # 讲AIO和O_DIRECT的关系
https://blog.csdn.net/yiyeguzhou100/article/details/106289024 # 内核源码调用路径
https://github.com/littledan/linux-aio?tab=readme-ov-file # littledan


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
