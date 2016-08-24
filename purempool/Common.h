#ifndef COMMON_H
#define COMMON_H
/**
 * 动态模式说明：
 * 一般（静态）模式下，信息块的大小是固定的，数组信息块的大小是索引块大小+信息块大小
 * 动态模式下，会在所有前置块后加一个”动态字节“ 用来说明前面的信息块大小，这时信息块大小是根据内存块大小动态分配的，数组信息块
 * 也和普通信息块相同
 * 动态模式首先被实现
 */
typedef unsigned long long qword;
typedef unsigned int dword;
typedef unsigned short word;
typedef unsigned char byte;
//这是预先检查的宏
#define testval(v) if(!v) return


#include <set>
#include <list>


#endif // COMMON_H
