#ifndef PUREMPOOL_H
#define PUREMPOOL_H

#include "Common.h"
/**
 * @brief 纯的内存池……注意这东西的 扩展功能和动态模式没完成,注意，此类非线程安全
 */
class Purempool
{

public:
    /**
     * @brief Memorypool 初始化内存池
     * @param isdynamic 是否开启动态块模式，动态模式说明在Common.h中
     * @param automp 是否开启自动扩展多池模式，当一个池中分配失败后自动到另一个池中分配，如果所有池分配失败则自动新建一个池
     * @param size 内存池大小
     * @param infs 信息块大小，信息块用于保存某指针指向的块大小，每分配一块内存则信息块数量+1,有效值 1 2 4 8，静态模式下有效
     * @param arins 数组索引块大小，用于保存数组成员数，数组信息块大小=索引块大小+元素信息块大小，静态模式下有效
     * 
     * 
     */
    Purempool(qword size,bool isdynamic=true,bool automp=false,byte infs=2,byte arins=2);

    ~Purempool();
    //分配
    void *New(qword size);

    void *Array(dword size,dword amount);

    //删除
    bool Delete(void *p);
    bool DArray(void *p);
    //手动删除


    //扩展
    /**
     * @brief Expand 扩展一个内存块
     * @param p 指针
     * @param fandb 扩展方向
     * @param count 扩展字节数
     * @return 成功与否,成功则指针会变成应该有的值,后扩展指针不变
     */
    bool Expand(void *&p,short fandb,dword count);
    //同上，elesize表示元素大小,count元素个数
    bool AExpand(void *&p,short fandb,dword elesize,dword count);
    //同上，自动扩展，先后后前,返回0代表后扩展成功,1代表前扩展成功，2代表不成功
    short AutoExpand(void *,dword count);
    short AutoAExpand(void *,dword count);

    void SetAutoPoolSize(qword size);//设置自动增加的池大小，自动增加模式下有效

    //池相关
    bool AddPool(qword size);   //返回成功与否
    dword FreePool();    //返回释放的池的数量
    

private:

    struct blockinfo
    {
        byte *startp;//块指针
        qword size;//块大小

    };

    struct mempool
    {
        mempool& operator =(mempool&)=delete;
        mempool(mempool &)=delete;
        mempool()=default;
        byte *poolp;    //池头指针
        qword size; //池大小
        std::list<blockinfo> fblist;    //分配表，即自由块列表
        qword maxfb;    //最大自由块大小
        qword maxsum;   //最大块数量
    };


    bool isdym;
    bool automp;
    byte infs,arins;
    qword epsize;//自动扩展池大小，为最后一次添加内存池的大小
    
    std::list<mempool *> pools;




    mempool *CreatePool(qword size);
    //没有检查，慎用
    byte *CreateMemblock(mempool *cpo, blockinfo &b, qword size);
    
    void *SetInfoBlock(byte *tbp, qword size);   //设置信息块的值,返回数据指针
    void *SetAInfoBlock(byte *tbp,qword size,qword arsize);  //设置数组信息块的值，返回数据指针
    void *GetInfoBlockPtr(void *p,qword &val); //根据数据指针获取信息块指针,信息块的值保存在qword中
    void *GetAinfoBlockPtr(void *p,qword &val,qword &sum);   //根据数据指针获取信息块指针,元素信息块的值保存在val中，索引块的值在 sum中
    void *GenSetInfo(byte *tbp,qword data,byte sinfo);  //通用设置函数，返回设置后的块后面的指针
    void *GenGetInfo(void *p,qword &val,byte sinfo);   //通用取得函数，返回完整指针
    void InsertToBL(blockinfo &b,mempool *cpo); //将一个自由块信息结构插入 一个池的块表中,并保持从小到大的有序状态,可合并则自动合并
    
    byte *BaseNew(qword size);  //基础新建函数不处理信息块
    bool BaseDelete(void *p,byte size);   //基础删除函数，不涉及信息块
    
    bool TestSize(qword size, byte sf);
    
};

#endif // PUREMPOOL_H
