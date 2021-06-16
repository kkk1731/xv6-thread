# xv6-thread

## 概要
呼び出し元とは別のページテーブルを持つスレッドを作成する。このとき、仮想メモリについて、

・スタックポインタが存在するページとそれより小さいアドレスをもつページ
→呼び出し元ページテーブルの物理メモリをコピーし、別の物理アドレスを指すようにする

・スタックポインタが存在するページより大きいアドレスを持つページ
→呼び出し元ページテーブルと同じ物理アドレスを指すようにする

という実装にした。すったくがあるページのみをコピーしてもエラーが起き、それより上のページをコピーしたらうまくいった。
本当は、呼び出し元のメモリを新たに確保・そこにスレッドのスタック領域を作成、スレッド側のスタックポインタを移動することによってページテーブルを共有したまま実装したかったが、スタックポインタのみを移動させてもエラーが起きたりしたのであきらめた。

## proc.c
実装した関数の概要を説明する。

### thread_create関数
fork()関数のページテーブルを丸々コピーする部分を、ページテーブルのスタックポインタとそれ以下の部分のみコピーし、その他は呼び出し元と同じ物理アドレスを指すようにした関数。
growproc_nthread(np) 関数を呼び出し、さらにその中でuvmalloc_thread(p->pagetable, np->pagetable, p->sz, p->trapframe->sp)を呼んでいる。その関数はvm.cで説明する。

## vm.c
### uvmalloc_thread関数
ページテーブルのスタックポインタとそれ以下の部分のみコピーし、その他は呼び出し元と同じ物理アドレスを指すようにする部分.
下のような感じで実装している。

sp:スタックポインタ
i：仮想アドレス（０からPGSIZEずつインクリメントされている）
mem：メモリの内容

スタックポインタ以下なら呼び出し元のpa(多分phisical address)の内容をmemにコピーし、
スタックポインタ以上なら呼び出し元のpaのポインタをmemに渡す。

```
・・・
if(i <= sp){
      if((mem = kalloc()) == 0)
        goto err;
      memmove(mem, (char *)pa, PGSIZE);
	//printf("after memmove\n");
	//copy stack page to added page
    } else{
      mem = (char *)pa;
    }
    
    if(mappages(new, i, PGSIZE, (uint64)mem, flags) != 0){
      kfree(mem);
      goto err;
    }
・・・
```

## スレッドからexitするときの注意
スレッドからexit するときには、ページテーブルのスタックポインタより大きなアドレスを持つページの変更が、呼び出し元に反映されないようにする必要がある。（thread生成時にはスタックポインタ以下のページしかコピーしていないため、それ以外のページは共有している)

よってthread用のexitするsystemcall  t_exit()を作り、ページテーブルの内容が呼び出し元に反映されないようにコピーする。

## 実験
下記プログラムで実験した。
2つのスレッドを生成し、それぞれで時間に差をつけて int *a を 1 インクリメントさせた。
うまくいけば、

スレッド1：*a=1　　
スレッド2： *a=2　　
スレッド3：*a=3　　
スレッド4：*a=4　　

となるはずである。


```
#include "kernel/types.h"
#include "kernel/stat.h"

#include "user/user.h"
#include "user/thread.h"

void thread_test(void *a){
	*((int *)a) = *((int *)a)+1;
	printf("pid:%d in thread  0s : a = %d\n", getpid(), *((int *)a));
	sleep(5);
	
	*((int *)a) = *((int *)a)+1;
	
	printf("pid:%d in thread 10s : a = %d\n", getpid(), *((int *)a));
	return;
}

int main(void){
	int ret1, ret2;
	int *a = malloc(sizeof(int));
	*a = 0;

	ret1 = thread(thread_test, (void *)a);
	sleep(2);
	ret2 = thread(thread_test, (void *)a);
	join(&ret1);
	join(&ret2);
	printf("join end\n");
	exit(0);
}
```


## 結果

下のプログラムで実験した。
できた。

```
$ ./test_thread
child thread
pid:4 in thread  0s : a = 1
child thread
pid:5 in thread  0s : a = 2
pid:4 in thread 10s : a = 3
end function
pid:5 in thread 10s : a = 4
end function
join end
```
