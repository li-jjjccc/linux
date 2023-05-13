#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

#define Test_CPP_1 0
#define Test_CPP_2 0
#define Test_CPP_3 0
#define Test_CPP_4 1

/*******************************************************************************************************/
#if Test_CPP_1  // 函数指针数组测试
#define N 3

typedef struct List {
} List;

List* A(const char* str, List** head);
List* B(const char* str, List** head);
List* C(const char* str, List** head);

List* A(const char* str, List** head) {
    printf("Func A\n");
    return NULL;
}
List* B(const char* str, List** head) {
    printf("Func B\n");
    return NULL;
}
List* C(const char* str, List** head) {
    printf("Func C\n");
    return NULL;
}

typedef struct Test {  // 函数指针数组
    List* (*raw_parse_hook[N])(const char*, List**);
} Test;
Test g_instance = {A, B, C};

int main(int argc, char* argv[]) {
    List* (*parse_hook)(const char*, List**) = NULL;

    for (int id = 0; id < N; id++) {
        parse_hook =
            (List * (*)(const char*, List**)) g_instance.raw_parse_hook[id];
        parse_hook(NULL, NULL);
    }
    return 0;
}
/*******************************************************************************************************/
#elif Test_CPP_2  // Vector数组初始化与否的速度测试
#define N 1000000
struct AdItem {
    AdItem() = default;
    AdItem(int x, int y, int z) { x_ = x, y_ = y, z_ = z; }
    int x_, y_, z_;
};
void TimeReport(const std::function<void()> &f1,
                const std::function<void()> &f2) {
    auto start = std::chrono::high_resolution_clock::now();
    f1();
    auto end = std::chrono::high_resolution_clock::now();

    /*
    std::cout<<"allocation done in
    "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<"ms"<<std::endl;
  */
    printf("allocation done in %d\n", end - start);

    start = std::chrono::high_resolution_clock::now();
    f2();
    end = std::chrono::high_resolution_clock::now();

    /*
    std::cout<<"assignment done in
    "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<"ms"<<std::endl;*/
    printf("assignment done in %d\n", end - start);
}
void uninitialized_performance() {
    std::vector<AdItem> v;
    TimeReport([&v]() { v.reserve(N); },
               [&v]() {
                   for (int i = 0; i < N; i++) {
                       v[i] = {i + 2, i + 4, i + 6};
                   }
               });
}
void initialized_performance() {
    std::vector<AdItem> v;
    TimeReport(
        [&v]() {
            v.assign(N, AdItem{0, 0, 0});
        },
        [&v]() {
            for (int i = 0; i < N; i++) {
                v[i] = {i + 2, i + 4, i + 6};
            }
        });
}
int main(int argc, char *argv[]) {
    uninitialized_performance();
    initialized_performance();
    return 0;
}
/*******************************************************************************************************/
#elif Test_CPP_3  // CPP 自定义实现动态扩容的string
// https://mp.weixin.qq.com/s/KLkvWgtaRrE5UmwH5znDQA
static const size_t c_string_min_size = 32;
struct c_string {
    char* str;
    size_t alloced;
    size_t len;
};
typedef struct c_string c_string_t;

c_string_t* c_string_create() {  // 创建字符串
    c_string_t* cs;
    cs = (c_string_t*)calloc(1, sizeof(*cs));

    cs->str = (char*)malloc(c_string_min_size * sizeof(char));
    *cs->str = '\0';

    cs->alloced = c_string_min_size;
    cs->len = 0;

    return cs;
}
void c_string_t_destory(c_string_t* cs) {  // 删除字符串
    if (cs) {
        free(cs->str);
        free(cs);
    }
}

static void c_string_ensure_space(c_string_t* cs, size_t add_len) {  // 内部扩容
    if (cs == NULL || add_len == 0) {
        return;
    }
    if (cs->alloced >= cs->len + add_len + 1) {
        return;
    }

    while (cs->alloced < cs->len + add_len + 1) {
        cs->alloced << 1;
        if (cs->alloced == 0) {
            cs->alloced--;
        }
    }
    cs->str = (char*)realloc(cs->str, cs->alloced);
}
void c_string_t_append_str(c_string_t* cs, const char* str,
                           size_t len) {  // 尾部追加字符串
    if (cs == NULL || str == NULL || *str == '\0') {
        return;
    }
    if (0 == len) {
        len = strlen(str);
    }

    c_string_ensure_space(cs, len);
    memmove(cs->str + cs->len, str, len);
    cs->len += len;
    cs->str[cs->len] = '\0';
}
void c_string_t_append_char(c_string_t* cs, char c) {  // 尾部追加字符
    if (cs == NULL) return;
    c_string_ensure_space(cs, 1);
    cs->str[cs->len] = c;
    cs->len++;
    cs->str[cs->len] = '\0';
}
void c_string_t_append_int(c_string_t* cs, int val) {  // 尾部追加整数
    if (cs == NULL) return;
    char str[12];
    snprintf(str, sizeof(str), "%d", val);
    c_string_t_append_str(cs, str, 0);
}

void c_string_front_str(c_string_t* cs,const char*str,size_t len)
{   // 在头部插入字符串：
    if(cs==NULL || str==NULL || *str=='\0')
    {
        return;
    }
    if(len==0)
    {
        len==strlen(str);
    }

    c_string_ensure_space(cs,len);
    memmove(cs->str+len,cs->str,len);
    memmove(cs->str,str,len);
    cs->len+=len;
    cs->str[cs->len]='\0';
}
void c_string_front_char(c_string_t* cs,const char c)
{   // 在头部插入字符：
    if(cs==NULL || c=='\0')
    {
        return;
    }
    

    c_string_ensure_space(cs,1);
    memmove(cs->str+1,cs->str,1);
    cs->str[0]=c;
    cs->len+=1;
    cs->str[cs->len]='\0';
}
void c_string_front_int(c_string_t* cs,int val)
{   // 在头部插入整数
    if(cs==NULL)
    {
        return;
    }

    char str[12];
    snprintf(str,sizeof(str),"%d",val);
    c_string_front_str(cs,str,0);
}

void c_string_truncate(c_string_t*cs,size_t len)
{   // 裁剪字符串：
    if(cs==NULL || len>=cs->len)
    {
        return;
    }
    cs->len=len;
    cs->str[cs->len]='\0';

}
void c_string_clear(c_string_t* cs)
{
    if(cs==NULL)
    {
        return;
    }
    c_string_truncate(cs,0);
}

void c_string_drop_begin(c_string_t*cs,size_t len)
{
    if(cs==NULL || len==0)
    {
        return;
    }
    if(len>=cs->len)
    {
        c_string_clear(cs);
        return;
    }
    cs->len-=len;
    memmove(cs->str,cs->str+len,cs->len+1);
}
void c_string_drop_end(c_string_t*cs,size_t len)
{
    if(cs==NULL || len==0)
    {
        return;
    }
    if(len>=cs->len)
    {
        c_string_clear(cs);
        return;
    }
    cs->len-=len;
    cs->str[cs->len]='\0';
}
size_t c_string_len(const c_string_t* cs)
{
    return (cs==NULL)?(0):(cs->len);
}
const char* c_string_peek(const c_string_t* cs)
{
    if(cs==NULL)
    {
        return NULL;
    }
    return cs->str;
}
char* c_string_dump(const c_string_t* cs,size_t* len)
{
    if(cs==NULL)
    {
        return NULL;
    }
    if(len!=NULL)
    {
        *len=cs->len;
    }
    char* out=(char*)malloc(sizeof(char)*(cs->len+1));
    memcpy(out,cs->str,cs->len+1);
    return out;
}
int main(int argc, char* argv[]) { return 0; }
/*******************************************************************************************************/
#elif Test_CPP_4 


int main(int argc, char* argv[])
{


    return 0;
}

/*******************************************************************************************************/
#endif