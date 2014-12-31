#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <map>
#include <dos.h>
#include <conio.h>
#include <setjmp.h>

using namespace std;

jmp_buf env,wrong;
FILE *tfp=fopen("F:\\program\\data\\time.txt","r");
FILE *ifp=fopen("F:\\program\\data\\input.txt","r");
char *quexi="缺席";
char *a_name="姓名";
char *a_number="工号";
char *chidao="迟到";
char *zaotui="早退";
char *zhengchang="正常";

struct time_i
{
    unsigned char t_hour;
    unsigned char t_min;
    unsigned char t_sec;
};

struct time_i gettime()
{
    time_t a;
    time(&a);
    struct tm *b=gmtime(&a);
    time_i c;
    c.t_hour=b->tm_hour;
    c.t_min=b->tm_min;
    c.t_sec=b->tm_sec;
    return c;
}

int getday()
{
    time_t a;
    time(&a);
    struct tm *b=gmtime(&a);
    return b->tm_mday;
}

int getmaxday(struct tm a)
{
    if(a.tm_mon==1)
    {
        int n=a.tm_year+1900;
        if(n&0b11 || ((!n%100) && n%400))
        {
            return 28;
        }
        else
        {
            return 29;
        }
    }
    else
    {
        switch(a.tm_mon)
        {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            return 31;
        case 1:
        case 3:
        case 5:
        case 8:
        case 10:
            return 30;
        default:
            longjmp(env,4);
        }
    }
};

int getmaxday()
{
    time_t a;
    time(&a);
    struct tm *b=gmtime(&a);
    return getmaxday(*b);
};


class qingkuang
{
public:
    unsigned char chuxin;
    unsigned char queqin;
    unsigned char chidao;
    unsigned char zaotui;
    qingkuang()
    {
        chuxin=0;
        queqin=0;
        chidao=0;
        zaotui=0;
    }
};

class manage
{
public:
    char name[16];
    char number[32];
    bool sex;
    char bumen[32];
    struct time_i first[31];
    struct time_i second[31];
    manage(char *name,char *number,bool sex,char *bumen)
    {
        strcpy(this->name,name);
        strcpy(this->number,number);
        this->sex=sex;
        strcpy(this->bumen,bumen);
        for(int i=0; i<31; i++)
        {
            first[i].t_hour=25;
        }
    }
    manage()
    {
    }
    void qiandao()
    {
        int day=getday()-1;
        if(first[day].t_hour==25)
        {
            first[day]=gettime();
        }
        else
        {
            second[day]=gettime();
        }
    }
    qingkuang getqingkuang()
    {
        qingkuang a;
        int n=getmaxday();
        for(int i=0; i<n; i++)
        {
            if(first[i].t_hour==25)
            {
                a.queqin++;
                continue;
            }
            if(first[i].t_hour>9)
            {
                a.chidao++;
            }
            if(second[i].t_hour>17)
            {
                a.zaotui++;
            }
            a.chuxin++;
        }
        return a;
    }
    void print()
    {
        printf("%s %s %c %s",name,number,sex?'f':'m',bumen);
    }
    void printdet(FILE *fp)
    {
        int i;
        int n=getmaxday();
        for(i=0; i<n; i++)
        {
            fprintf(fp,"%d\t",i+1);
            if(first[i].t_hour==25)fprintf(fp,"%s\t%s\n",quexi,quexi);
            else
            {
                char *a,*b;
                if(first[i].t_hour>9) a=chidao;
                else a=zhengchang;
                if(second[i].t_hour<17) b=zaotui;
                else b=zhengchang;
                fprintf(fp,"%d:%s\t%d:%s\n",first[i].t_hour,a,second[i].t_hour,b);
            }
        }
    }
    void clean()
    {
        for(int i=0; i<31; i++)
        {
            first[i].t_hour=25;
        }
    }
};

int cmp(const manage *a,const manage *b)
{
    return strcmp(a->number,b->number);
}

class manage_lib
{
public:
    FILE *fp;
    manage *start;

    unsigned int num;
    unsigned int max;
    manage *begin()
    {
        return start;
    }
    manage *end()
    {
        return start+num;
    }
    manage_lib(FILE *fp)
    {
        this->fp=fp;
        fseek(fp,0,2);
        int n=ftell(fp);
        if(n==0)
        {
            start=(manage *)malloc(sizeof(manage));
            max=1;
            num=0;
            return;
        }
        start=(manage *)malloc(n);
        max=n/sizeof(manage);
        num=max;
        if(n%sizeof(manage)) longjmp(env,1);
        rewind(fp);
        fread(start,max,sizeof(manage),fp);
    }
    void add_manage( manage a)
    {
        if(max==num)
        {
            max<<=1;
            printf("%d",max);
            start=(manage *)realloc(start,max*sizeof(manage));
        }
        start[num++]=a;
    }
    manage *find(char *str)
    {
        int a=0,b=num/2,c=num;
        while(c-a>1)
        {
            int n=strcmp(start[b].number,str);
            if(n>0)
            {
                c=b;
                b=(c+a)/2;
            }
            else if(n<0)
            {
                a=c;
                b=(c+a)/2;
            }
            else
            {
                return &start[b];
            }
        }
        if(strcmp(start[a].number,str)==0)
        {
            return &start[a];
        }
        if(strcmp(start[c].number,str)==0)
        {
            return &start[c];
        }
        longjmp(env,2);
    }
    void save()
    {
        fseek(fp,0,0);
        qsort(start,num,sizeof(manage),(int (*)(const void *,const void *))cmp);
        fwrite(start,sizeof(manage),num,fp);
        fflush(fp);
    }
    void clean()
    {
        for(manage &a:*this)
        {
            a.clean();
        }
    }
};

manage_lib *all_lib;

void readinfo(manage_lib *lib)
{
    system("cls");
    puts("input the manage information:\nname number sex bumen");
    char a[16],b[32],c,d[32];
    fscanf(ifp,"%s %s %c %s",a,b,&c,d);
    lib->add_manage(manage(a,b,c=='m'?1:0,d));
}

void sign(manage_lib *lib)
{
    system("cls");
    printf("input the number\n");
    char str[16];
    scanf("%s",str);
    if(setjmp(env))
    {
        printf("no %s in the lib\nplease reinput\n",str);
        scanf("%s",str);
    }
    lib->find(str)->qiandao();
}

void printhelp()
{
    system("cls");
    puts("1.input the manager information");
    puts("2.sign");
    puts("3.get the info");
    puts("4.find the manager");
    puts("5.exit");
}

void findi(manage_lib *lib){
  system("cls");
  printf("%s\n","input the number");
  char a[1024];
  if(int n=setjmp(env)){
    printf("manage no found\n");
    printf("please reinput\n");
  }
  fgets(a,1024,stdin);
  a[strlen(a)-1]=0;
  lib->find(a)->printdet(stdout);
  getchar();
}

void tongji(manage_lib *lib)
{
    system("cls");
    printf("%s\t%s\t%s\t%s\t%s\t%s\n",a_name,a_number,chidao,zaotui,quexi,zhengchang);
    for(manage &a:*lib)
    {
        qingkuang b=a.getqingkuang();
        printf("%s\t%s\t%d\t%d\t%d\t%d\n",a.name,a.number,b.chidao,b.zaotui,b.queqin,b.chuxin);
    }
    getchar();
}

int main()
{
    FILE *fp=fopen("F:\\program\\data\\manage.bin","rb+");
    if(int n=setjmp(env))
    {
        printf("wrong!:%d\n",n);
        return 0;
    }
    manage_lib lib(fp);
    all_lib=&lib;
    char ch;
    printhelp();
    while((ch=getche())-'5')
    {
        switch(ch)
        {
        case '1':
            readinfo(&lib);
            break;
        case '2':
            sign(&lib);
            break;
        case '3':
            tongji(&lib);
            break;
        case '4':
            findi(&lib);
            break;
        case '5':
            goto fin;
        }
        printhelp();
    }
    fin:
    lib.save();
}
