#include <iostream>
#include <purempool.h>
#include <ctime>
extern "C"
{
#include <Windows.h>
}

#include <thread>
#include <mutex>
using namespace std;


void fun(Purempool *pool,dword d,qword **qp);
int main()
{
    cout<<"按任意键继续……"<<endl;
    cin.get();
    DWORD st,et;
    Purempool pool(10000000,false,false,1,1);
    
    cout<<"按任意键继续……"<<endl;
    cin.get();
    
    qword **qp=new qword*[1000000];
    
    cout<<"按任意键继续……"<<endl;
    cin.get();
    
    cout<<"单线程一百w次8字节内存分配(消耗空间为实际占用空间-8M):"<<endl;
    st=GetTickCount();
    for(int i=0;i<1000000;++i)
    {
        qp[i]=(qword *)pool.New(8);
    }
    et=GetTickCount();
    cout<<"消耗时间："<<et-st<<endl;
    
    cin.get();
    
    cout<<"现在进行单线程一百w次8字节释放:"<<endl;
    st=GetTickCount();
    for(int i=0;i<1000000;++i)
    {
        if(!pool.Delete(qp[i])) return 1;
    }
    et=GetTickCount();
    cout<<"消耗时间："<<et-st<<endl;   
    cin.get();
    cout<<"再分配8M:"<<endl;
    for(int i=0;i<1000000;++i)
    {
        qp[i]=(qword *)pool.New(8);
    }
    dword tsum=0;
    for(int i=0;i<1000000;++i) if(qp[i]==nullptr) ++tsum;
    cout<<"计数结果 失败数:"<<tsum<<endl;
    
    cout<<endl<<"按任意键继续4线程分配……"<<endl;
    cin.get();
    cout<<"现在进行4线程一百w次8字节分配:"<<endl;

    qword **qp2=new qword*[1000000];
    qword **qp3=new qword*[1000000];
    qword **qp4=new qword*[1000000];
    Purempool pool6(10000000,false,false,1,1);
    Purempool pool2(10000000,false,false,1,1);
    Purempool pool3(10000000,false,false,1,1);
    Purempool pool4(10000000,false,false,1,1);
    st=GetTickCount();
    thread t1(fun,&pool6,1000000,qp);
     thread t2(fun,&pool2,1000000,qp2);
     thread t3(fun,&pool3,1000000,qp3);
      thread t4(fun,&pool4,1000000,qp4);
     t1.join();
     t2.join();
     t3.join();
     t4.join();
    et=GetTickCount();
    cout<<"消耗时间："<<et-st<<endl;  
    
    cout<<"现在进行Addpool测试:"<<endl;
        cin.get();
    cout<<"单线程用的10M池已经分配8M，再分配8M："<<endl;
    qword **qpt=new qword*[1000000];
    for(int i=0;i<1000000;++i)
    {
        qpt[i]=(qword *)pool.New(8);
    }
    dword sum=0;
    for(int i=0;i<1000000;++i) if(qpt[i]==nullptr) ++sum;
    cout<<"计数结果 失败数:"<<sum<<endl;
    cout<<"现在分配一个10M池"<<endl;
    pool.AddPool(10000000);
    cout<<"再次测试"<<endl;
    
    for(int i=0;i<1000000;++i)
    {
        qpt[i]=(qword *)pool.New(8);
    }
    sum=0;
    for(int i=0;i<1000000;++i) if(qpt[i]==nullptr) ++sum;
    cout<<"计数结果 失败数:"<<sum<<endl;
    
    cin.get();
    return 0;
}

void fun(Purempool *pool,dword d,qword **qp)
{
    for(int i=0;i<d;++i)
    {
        qp[i]=(qword *)pool->New(8);
    }
}
