# 好文章

https://www.kernel.org/doc/html/latest/core-api/wrappers/memory-barriers.html # 内核官文
https://www.cnblogs.com/fanguang/p/16643434.html # 上文的中文翻译，上文英文太长了
https://blog.csdn.net/weixin_45030965/article/details/132852641 # 网友好文
https://cloud.tencent.com/developer/article/1801450 # 网友好文
https://www.kernel.org/doc/html/latest/process/volatile-considered-harmful.html # linux不赞成使用volatile关键字
https://lwn.net/Articles/233482/ # 上文的引用文，linus大神讲述为啥不用volatile
https://cloud.tencent.com/developer/article/1801450 # 讲到观察编译器乱序

# 编译器乱序

编译器乱序通过barrier()搞定。
来源于：https://blog.csdn.net/linuxweiyh/article/details/79139766

barrier()语句的意思是：下面的语句不能跑上面去，上面的语句也不能跑下面来。

READ_ONCE() 和 WRITE_ONCE()也可以用来预防编译器乱序，但是它只防止所有READ_ONCE()/WRITE_ONCE()之前的顺序被搞乱，其他变量它就不管了，效果没有barrier()好。

# CPU乱序

要通过smp_wmb() smp_rmb() smp_mb()来解决

# 相关代码

```c
// compiler.h:
/* The "volatile" is due to gcc bugs */
# define barrier() __asm__ __volatile__("": : :"memory")

#define READ_ONCE(x) (*(volatile typeof(x) *)&(x))

#define WRITE_ONCE(x, val) \
({							\
	union { typeof(x) __val; char __c[1]; } __u =	\
		{ .__val = (__force typeof(x)) (val) }; \
	__write_once_size(&(x), __u.__c, sizeof(x));	\
	__u.__val;					\
})

```

# C语言中的原子操作

C语言的原子操作应该不用担心编译器乱序还是CPU乱序，应该全包含中它的原子语义中去来

## 好文章

https://www.cnblogs.com/sunddenly/articles/15389917.html # 网友文章
https://blog.csdn.net/bill_xiang/article/details/51286854 # 网友文章
https://blog.csdn.net/zhangxiao93/article/details/42966279 # 网友文章
https://runebook.dev/zh/docs/gcc/extended-asm # gcc手册中文版

## 读->读不允许乱序、写->写不允许乱序

![image-20240625212411485](https://raw.githubusercontent.com/zp001paul/myarticle/main/img/image-20240625212411485.png?token=A6BGZFNZVCKTLGVU74SVKBLGPLCT6)

## 读->写不允许乱序

![image-20240625212630331](https://raw.githubusercontent.com/zp001paul/myarticle/main/img/image-20240625212630331.png?token=A6BGZFPCLZZKZI2OJJ3HMC3GPLC4I)

## 写->读 如果位置相同不允许乱序，如果位置不同允许乱序

![image-20240625213010093](https://raw.githubusercontent.com/zp001paul/myarticle/main/img/image-20240625213010093.png?token=A6BGZFN2DLA5QY3O4JNLUELGPLDJ6)

## 所有写入都是全局可见

![image-20240625213627350](https://raw.githubusercontent.com/zp001paul/myarticle/main/img/image-20240625213627350.png?token=A6BGZFJZXEDBWGQGX7PCSRTGPLEBS)

## 总结

只有写->读一种情况可能乱序

# gcc手册提到

6.46 章节有说

# golang 原子性

## 好文章

https://blog.csdn.net/qq_43598865/article/details/124477353 # 网友写得很好
