#include <chrono>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
using namespace std;
/* Effective C++  Basics 基础议题 */
// https://qppw4bc6rk.feishu.cn/wiki/wikcnuaqT43nix9cnfzpxTvYjxe

/*
指针与引用 Pointer Reference
不建议使用c语言的类型转换方法,应该static_cast dynamic_cast const_cast reinterpret_cast进行转换
不用以多态的方法处理数组
*/





class Base_1{
public:
    int a;
    double b;
    Base_1(){a=0,b=3.14;}
    Base_1(int _a,double _b):a(_a),b(_b){}
    Base_1(const Base_1& base){this->a=base.a,this->b=base.b;}
    Base_1(Base_1&& base){this->a=base.a,this->b=base.b;}
};
class Derived_1:public Base_1{
protected:
    std::string c;
public:
    Derived_1(){}
    Derived_1(const Derived_1& _c)
    {
        this->a=_c.a+3;
        this->b=_c.b-0.14;
    }
    Derived_1(Derived_1&& _c)
    {
        this->a=_c.a+3;
        this->b=_c.b-0.14;
    }
};

void test_1()
{
    // static_cast
    int a=9;
    double b=5.5678;
    double c=b+static_cast<double>(a);
    
    std::cout<<"c:"<<c<<std::endl;

    Derived_1 d1;
    Derived_1 d2(d1);
    Derived_1 d3(std::move(d2));
    printf("a=%d,b=%f\n",d1.a,d1.b);
    printf("a=%d,b=%f\n",d2.a,d2.b);
    printf("a=%d,b=%f\n",d3.a,d3.b);

    Base_1 b1=static_cast<Base_1>(d3);
    printf("a=%d,b=%f\n",b1.a,b1.b);

}
void test_2()
{   // const_cast
    const string str="Hello ";
    string& str_1=const_cast<string&>(str); 
    str_1.append("world");
    cout<<str_1<<endl;


    string* str_2=const_cast<string*>(&str); cout<<*str_2<<endl;
}

class BST{
public:
    char c;
};
class balanced:public BST{
public:
    int i;
};
void printBSTarray(ostream& s,const BST array[],int numElements)
{
    for(int i=0;i<numElements;i++)
    {
        s<<array[i].c;
    }
    s<<endl;
}
void test_3()
{
    // 以基类指针，操作派生类中的数组会出错
    const int numElements=4;
    BST b[numElements];
    for(int i=0;i<numElements;i++)
    {
        b[i].c='e'+i;
    }
    printBSTarray(cout,b,numElements);

    balanced b_[numElements];
    for(int i=0;i<numElements;i++)
    {
        b_[i].c='a'+i;
        b_[i].i=i;
    }
    printBSTarray(cout,b_,numElements); // error
}


class Construct{
public:
    Construct(int a){};
};
void test_4()
{
    // 非必要不要设定default constructor(ctor)
    // Construct c[9];
    // Construct& c_=new Construct[9];
    // Construct c;
    Construct c(4);
    Construct _c[2]={Construct(1),Construct(2)};
    // 也可使用指针数组

}
int main(int argc, char* argv[])
{
    test_4();
    /*
    https://zhuanlan.zhihu.com/p/368267441

    dynamic_cast专门用于将多态基类的指针或引用强制转换为派生类的指针或引用，而且能够检查转换的安全性。对于不安全的指针转换，转换结果返回 NULL 指针。

    reinterpret_cast 用于进行各种不同类型的指针之间、不同类型的引用之间以及指针和能容纳指针的整数类型之间的转换，reinterpret_cast 转换时，执行的过程是逐个比特复制的操作。 也可由于函数指针转换
    */
    return 0;
}


/*******************************************************************************************************/
