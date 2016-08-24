#include "purempool.h"
#include <algorithm>


Purempool::Purempool(qword size,bool isdynamic, bool automp, byte infs, byte arins)
{
    auto tp=CreatePool(size);
    pools.push_back(tp);
    //内存池创建完毕
    epsize=size; //设置最后添加的池的大小
    //保存属性
    this->isdym=isdynamic;
    this->automp=automp;
    this->infs=infs;
    this->arins=arins;
    
}

Purempool::~Purempool()
{
    //释放所有内存池
    for(mempool *pp:pools)
    {
        delete[] pp->poolp;
        delete pp;
    }
}

void *Purempool::New(qword size)
{

    if(!TestSize(size,infs)) return nullptr;
    byte *t=BaseNew(size+infs);
    if(t==nullptr)  return nullptr;
    return this->SetInfoBlock(t,size);
}

void *Purempool::Array(dword size, dword amount)
{
    if(!TestSize(size,infs)) return nullptr;
    if(!TestSize(amount,arins)) return nullptr;
    byte *t=BaseNew(size*amount+infs+arins);
    if(t==nullptr) return nullptr;
    return SetAInfoBlock(t,size,amount);
}

bool Purempool::Delete(void *p)
{
    return BaseDelete(p,infs); 
}

bool Purempool::DArray(void *p)
{
    return BaseDelete(p,infs+arins);
}


bool Purempool::Expand(void *&p, short fandb, dword count)
{
    return 0;
}

bool Purempool::AExpand(void *&p, short fandb, dword elesize, dword count)
{
    return 0;
}

short Purempool::AutoExpand(void *, dword count)
{
    return 0;
}

short Purempool::AutoAExpand(void *, dword count)
{
    return 0;
}

void Purempool::SetAutoPoolSize(qword size)
{
    epsize=size;
}

bool Purempool::AddPool(qword size)
{
    mempool *tp=CreatePool(size);
    if(tp==nullptr) return false;
    pools.push_back(tp);
    return true;
}

dword Purempool::FreePool()
{
    dword sum;
    for(auto i=pools.begin();i!=pools.end();++i)
    {
        mempool *tp=*i;
        if(tp->fblist.size()==1)
        {
            blockinfo &tb=tp->fblist.front();
            if(tb.startp==tp->poolp&&tb.size==tp->size) //可以释放
            {
                delete[] tp->poolp;
                delete tp;
                pools.erase(i);
                ++sum;
            }
        }
    }
    return sum;
}



Purempool::mempool *Purempool::CreatePool(qword size)
{
    mempool *tp=new mempool();
    tp->poolp=new byte[size];
    tp->size=size;
    tp->maxfb=size;
    blockinfo info;
    info.size=size;
    info.startp=tp->poolp;
    tp->fblist.push_back(info);
    tp->maxsum=1;
    return tp;
}


byte *Purempool::CreateMemblock(mempool *cpo,blockinfo &b, qword size)
{
    auto tsize=b.size;
    byte *ret=b.startp;
    b.startp+=size;
    b.size-=size;
    
    //如果是最大块，那么分配前必须要进行一些处理
    if(tsize==cpo->maxfb)
    {
        --cpo->maxsum;
        if(cpo->maxsum==0)    //如果原来数值的最大块已经没有，就重新选定
        {
            //首先搜索出最大的块的大小
            qword cmax=0;//预先置0，以应对没有自由块的情况
            qword csum=0;
            for(auto titr=cpo->fblist.begin();titr!=cpo->fblist.end();++titr)
            {
                blockinfo &tb=*titr;
                if(tb.size==cmax) csum++;//如果搜索到一个当前认为的最大块就把计数器加1
                else if(tb.size>cmax)
                {
                    cmax=tb.size;
                    csum=1;
                    //如果有新的当前最大块就重新开始计数
                }
            }
            cpo->maxfb=cmax;
            cpo->maxsum=csum;
        }
    }

    return ret;
}

void *Purempool::SetInfoBlock(byte *tbp, qword size)
{
    //处理信息块
    //注意大量隐式转换,都是条件允许的
    return GenSetInfo(tbp,size,infs);
}

void *Purempool::SetAInfoBlock(byte *tbp, qword size, qword arsize)
{
    //处理信息块
    //注意大量隐式转换,都是条件允许的
    void *tp=GenSetInfo(tbp,size,infs);
    return GenSetInfo((byte *)tp,arsize,arins);
}

void *Purempool::GetInfoBlockPtr(void *p, qword &val)
{
    return GenGetInfo(p,val,infs);
}

void *Purempool::GetAinfoBlockPtr(void *p, qword &val, qword &sum)
{
    //倒过来的
    void *tp=GenGetInfo(p,sum,arins);
    return GenSetInfo((byte *)tp,val,infs);
}



void *Purempool::GenSetInfo(byte *tbp, qword data,byte sinfo)
{
    //处理块
    //注意大量隐式转换,都是条件允许的
    switch(sinfo)
    {
    case 1:
        *tbp=data;
        return tbp+1;
    case 2:
        *(word *)tbp=data;
        return tbp+2;
    case 4:
        *(dword *)tbp=data;
        return tbp+4;
    case 8:
        *(qword *)tbp=data;
        return tbp+8;
    default:
        return nullptr;
    }
}

void *Purempool::GenGetInfo(void *p, qword &val, byte sinfo)
{
    byte *tp=(byte *)p;
    switch(sinfo)
    {
    case 1:
        --tp;
        val=*tp;
        return tp;
    case 2:
        tp-=2;
        val=*tp;
        return tp;
    case 4:
        tp-=4;
        val=*tp;
        return tp;
    case 8:
        tp-=8;
        val=*tp;
        return tp;
    default:
        return nullptr;
    }
}

void Purempool::InsertToBL(Purempool::blockinfo &b, Purempool::mempool *cpo)
{
    for(auto i=cpo->fblist.begin();i!=cpo->fblist.end();++i)
    {
        blockinfo &t=*i;
        if(t.startp+t.size==b.startp)
        {
            t.size+=b.size;
            break;
        }
        if(t.startp>b.startp)
        {
            if(b.startp+b.size==t.startp)
            {
                t.startp-=b.size;
                break;
            }
            else
            {
                cpo->fblist.insert(i,b);
                break;
            }
        }
    }
}

byte *Purempool::BaseNew(qword size)
{
    mempool *cpool=nullptr;
    for(mempool *i:this->pools)
    {
        if(i->maxfb>=size)
        {
            cpool=i;
            break;
        }
    }
    if(!cpool)  //如果没有搜索到可用的池
    {
        if(automp)  //如果自动添加池功能开启就自动添加池
        {
            CreatePool(epsize);
        }
        else return nullptr;
    }
    byte *tbp=nullptr;
    //搜索可用块
    for(auto i=cpool->fblist.begin();i!=cpool->fblist.end();++i)
    {
        blockinfo &t=*i;
        if(t.size>=size)
        {
            tbp=CreateMemblock(cpool,t,size);
            if(t.size==0) cpool->fblist.erase(i);
            break;
        }
    }
    if(tbp==nullptr) return nullptr;
    return tbp;
}

bool Purempool::BaseDelete(void *p,byte size)
{
    //搜索所属池
    mempool *cpool=nullptr;
    for(mempool *i:pools)
    {
        if(i->poolp<=p&&i->poolp+i->size>p)//在这个池内
        {
            cpool=i;
            break;
        }
    }
    if(cpool==nullptr) return false;
    //处理自由块
    blockinfo bk;
    bk.startp=(byte *)GenGetInfo(p,bk.size,size);   //一次性设置两个数据
    bk.size+=size;  //要包括前缀块的大小
    //加入块表中
    this->InsertToBL(bk,cpool);
    //处理最大块和最大块计数
    if(bk.size>=cpool->maxfb)
    {
        if(bk.size>cpool->maxfb)
        {
            cpool->maxfb=bk.size;
            cpool->maxsum=0;
        }
        ++cpool->maxsum;
    }
    return true;
}

bool Purempool::TestSize(qword size,byte sf)
{
    switch(sf)
    {
    case 1:
        return size<256;
    case 2:
        return size<65536;
    case 4:
        return size<4294967296;
    case 8:
        return true;    //8就不用说了……
    default:
        return false;
    }
}

