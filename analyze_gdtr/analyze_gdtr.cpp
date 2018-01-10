// analyze_gdtr.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

/*
 * ����λ�ηֳ���ԭʼ�ṹ��
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
 * ���Ա�ԭʼ�ṹ��
 */
typedef struct _GDTR_ATTR
{
    unsigned int Type :  4;
    unsigned int DT   :  1;
    unsigned int DPL  :  2;
    unsigned int P    :  1;
    // unsigned int      :  4; // �ƶ������һ�У��൱�ڷ������λ
                               // �������ת��
    unsigned int AVL  :  1;
    unsigned int      :  1;
    unsigned int D    :  1;
    unsigned int G    :  1;
    unsigned int      :  4;
} GDTR_ATTR;

/*
 * ������Ľṹ��
 */
typedef struct _Analyze_GDTR
{
    unsigned int Base;
    unsigned int Limite;
    GDTR_ATTR    Attr;
} Analyze_GDTR;

/*
 * ��Ϊ������תʹ�õĽṹ��
 */
struct _GDTR_ATTR_TEMP
{
    unsigned char AttrM5;
    unsigned char AttrM6;
} g_GdtrAttrTemp;

/*
 * ����ͬλ�õĻ�ַ�ϲ�����
 */
#define MAKE_GDTR_BASE(nBaseL, nBaseM, nBaseH, nBase)\
    (nBase = (nBaseH << 24) + (nBaseM << 16) + nBaseL);

/*
 * ����ͬλ�õĴ�С�ϲ�����
 */
#define MAKE_GDTR_LIMITE(nLimiteL, nLimiteH, nLimite)\
    (nLimite = (nLimiteH << 16) + nLimiteL);

/*
 * // ��ԭʼ���Ա�����ת��Ϊ���Ա�ṹ��
 */
#define MAKE_GDTR_ATTR(data, attr)\
    g_GdtrAttrTemp.AttrM5 = data.AttrM5;\
    g_GdtrAttrTemp.AttrM6 = data.AttrM6;\
    (attr = *(GDTR_ATTR *)&(g_GdtrAttrTemp));

/*
 * ����GDTRԭʼ���ݽṹ��Ϊ�µĽṹ�壬������Gλ�����δ�С
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
 * ����ĺ��൱��������������
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

// �������Ա�
char* g_szGdtrAttrType[] =
{
    "ֻ��",
    "ֻ�����ѷ���",
    "��/д",
    "��/д���ѷ���",
    "ֻ����������չ",
    "ֻ����������չ���ѷ���",
    "��/д��������չ",
    "��/д��������չ�����Է���",
    "ִֻ��",
    "ִֻ�С��ѷ���",
    "ִ��/��",
    "ִ��/�����ѷ���",
    "ִֻ�С�һ�����",
    "ִֻ�С�һ����Ρ��ѷ���",
    "ִ��/����һ�����",
    "ִ��/����һ����Ρ��Է���",
};


void PrintGdtr(Analyze_GDTR aData)
{
    printf("\n");
    printf("�λ�ַ              ��%08x\n", aData.Base);
    printf("�δ�С(����Gλ������)��%x\n", aData.Limite);
    printf("�����ԣ�\n");
    printf("P    (����Чλ)��%x\n", aData.Attr.P);
    printf("DPL  (��Ȩ��)  ��%x\n", aData.Attr.DPL);
    printf("DT   (������)  ��%x\n", aData.Attr.DT);
    printf("TYPE (����)    ��%x (%s)\n", aData.Attr.Type, g_szGdtrAttrType[aData.Attr.Type]);
    printf("G    (����λ)  ��%x\n", aData.Attr.G);
    printf("D(��ջ��������)��%x\n", aData.Attr.D);
    printf("AVL(������λ)  ��%x\n", aData.Attr.AVL);
    printf("\n");
}


int main()
{
    unsigned long long gdtr = 0;
    MY_GDTR data;
    Analyze_GDTR aData;
    while (1)
    {
        printf("������8�ֽڵ�GDTR����(16����)(����0����)��");
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

