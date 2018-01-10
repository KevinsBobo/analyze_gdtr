// analyze_gdtr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*
 * 按照位段分出的原始结构体
 */
typedef struct _MY_GDTR
{
    unsigned int  LimiteL : 16;
    unsigned int  BaseL   : 16;
    unsigned int  BaseM   :  8;
    unsigned int  AttrM5  :  8;
    unsigned int  LimiteH :  4;
    unsigned int  AttrM6  :  4;
    unsigned int  BaseH   :  8;
} MY_GDTR;

/*
 * 属性表原始结构体
 */
typedef struct _GDTR_ATTR
{
    unsigned int Type :  4;
    unsigned int DT   :  1;
    unsigned int DPL  :  2;
    unsigned int P    :  1;
    // unsigned int      :  4; // 移动到最后一行，相当于放在最高位
                               // 方便后期转换
    unsigned int AVL  :  1;
    unsigned int      :  1;
    unsigned int D    :  1;
    unsigned int G    :  1;
    unsigned int      :  4;
} GDTR_ATTR;

/*
 * 解析后的结构体
 */
typedef struct _Analyze_GDTR
{
    unsigned int Base;
    unsigned int Limite;
    GDTR_ATTR    Attr;
} Analyze_GDTR;

/*
 * 作为解析中转使用的结构体
 */
struct _GDTR_ATTR_TEMP
{
    unsigned char AttrM5;
    unsigned char AttrM6;
} g_GdtrAttrTemp;

/*
 * 将不同位置的基址合并起来
 */
#define MAKE_GDTR_BASE(nBaseL, nBaseM, nBaseH, nBase)\
    (nBase = (nBaseH << 24) + (nBaseM << 16) + nBaseL);

/*
 * 将不同位置的大小合并起来
 */
#define MAKE_GDTR_LIMITE(nLimiteL, nLimiteH, nLimite)\
    (nLimite = (nLimiteH << 16) + nLimiteL);

/*
 * // 将原始属性表数据转换为属性表结构体
 */
#define MAKE_GDTR_ATTR(data, attr)\
    g_GdtrAttrTemp.AttrM5 = data.AttrM5;\
    g_GdtrAttrTemp.AttrM6 = data.AttrM6;\
    (attr = *(GDTR_ATTR *)&(g_GdtrAttrTemp));

/*
 * 解析GDTR原始数据结构体为新的结构体，并根据G位调整段大小
 */
#define MAKE_GDTR(data, aData)\
    MAKE_GDTR_BASE(data.BaseL, data.BaseM, data.BaseH, aData.Base)\
    MAKE_GDTR_LIMITE(data.LimiteL, data.LimiteH, aData.Limite)\
    MAKE_GDTR_ATTR(data, aData.Attr)\
    if(aData.Attr.G)\
    {\
      aData.Limite = (aData.Limite << 12) + 0xFFF;\
    }\
/*
 * 上面的宏相当于下面代码的作用
 */
/*
    aData.Base = (data.BaseH << 24) + (data.BaseM << 16) + data.BaseL;
    aData.Limite = (data.LimiteH << 16) + data.LimiteL;
    g_GdtrAttrTemp.AttrM5 = data.AttrM5;
    g_GdtrAttrTemp.AttrM6 = data.AttrM6;
    aData.Attr = *(GDTR_ATTR *)&(g_GdtrAttrTemp);
    if(aData.Attr.G)
    {
      aData.Limite = (aData.Limite << 12) + 0xFFF;
    }
*/

// 具体属性表
char* g_szGdtrAttrType[] =
{
    "只读",
    "只读、已访问",
    "读/写",
    "读/写、已访问",
    "只读、向下扩展",
    "只读、向下扩展、已访问",
    "读/写、向下扩展",
    "读/写、向下扩展’、以访问",
    "只执行",
    "只执行、已访问",
    "执行/读",
    "执行/读、已访问",
    "只执行、一致码段",
    "只执行、一致码段、已访问",
    "执行/读、一致码段",
    "执行/读、一致码段、以访问",
};


void PrintGdtr(Analyze_GDTR aData)
{
    printf("\n");
    printf("段基址              ：%08x\n", aData.Base);
    printf("段大小(根据G位修整后)：%x\n", aData.Limite);
    printf("段属性：\n");
    printf("P    (段有效位)：%x\n", aData.Attr.P);
    printf("DPL  (特权级)  ：%x\n", aData.Attr.DPL);
    printf("DT   (描述符)  ：%x\n", aData.Attr.DT);
    printf("TYPE (属性)    ：%x (%s)\n", aData.Attr.Type, g_szGdtrAttrType[aData.Attr.Type]);
    printf("G    (粒度位)  ：%x\n", aData.Attr.G);
    printf("D(堆栈段描述符)：%x\n", aData.Attr.D);
    printf("AVL(可利用位)  ：%x\n", aData.Attr.AVL);
    printf("\n");
}


int main()
{
    unsigned long long gdtr = 0;
    MY_GDTR data;
    Analyze_GDTR aData;
    while (1)
    {
        printf("请输入8字节的GDTR数据(16进制)(输入0结束)：");
        scanf_s("%llx", &gdtr);
        if (!gdtr)
        {
            break;
        }

        data = *(MY_GDTR*)&gdtr;

        MAKE_GDTR(data, aData);
        PrintGdtr(aData);
    }

    return 0;
}

