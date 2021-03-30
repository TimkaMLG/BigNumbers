#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BASE 1000000000
struct bn_s {
    int *body; // Тело большого числа
    int bodysize; // Размер массива body
    int sign; // Знак числа
};
typedef struct bn_s bn;
//сравнение модулей
int cmp(bn *t1, bn const *t2){
    if(t1->bodysize > t2->bodysize)
        return 1;
    if(t1->bodysize < t2->bodysize)
        return 0;
    int i = t1->bodysize-1;
    while(t1->body[i] == t2->body[i] && i >= 0){
        i--;
    }
    if(i == -1)
        return 1;
    if(t1->body[i] > t2->body[i])
        return 1;
    if(t1->body[i] < t2->body[i])
        return 0;
    return 0;
}
// Создать новое BN
bn *bn_new() {
    bn * r = malloc(sizeof (bn));
    if (r == NULL) return NULL;
    r->bodysize = 1;
    r->sign = 0;
    r->body = malloc(sizeof(int) * r->bodysize);
    if (r->body == NULL) {
        free(r);
        return NULL;
    }
    r->body[0] = 0;
    return r;
}
// Скопировать число
bn *bn_init(bn const *orig){
    bn * r = malloc(sizeof (bn));
    if (r == NULL) return NULL;
    r->bodysize = orig->bodysize;
    r->sign = orig->sign;
    r->body = malloc(sizeof(int) * r->bodysize);
    for(int i = 0; i < r->bodysize; i++)
        r->body[i] = orig->body[i];
    if(r->body == NULL){
        free(r);
        return NULL;
    }
    return r;
}
// Инициализировать значение BN деситичным представлением строки
int bn_init_string(bn *t, const char *init_string){
    int i=(int)strlen(init_string);
    char *sub = malloc(sizeof(char) * i);
    strcpy(sub,init_string);
    if(sub[0] == '-'){
        t->sign = 1;
        sub[0] = ' ';
    }
    t->body = realloc(t->body, sizeof(int) * ((i - 1 - t->sign)/9+1));
    t->bodysize = (i - 1 - t->sign)/9+1;
    for (int j = 0; i > t->sign; i-=9, j++) {
        sub[i] = 0;
        if(i >= 9)
            t->body[j] = atoi(sub+i-9);
        else
            t->body[j] = atoi(sub);
    }
    free(sub);
    return 0;
}
// Инициализировать значение BN представлением строки
// в системе счисления radix
/*int bn_init_string_radix(bn *t, const char *init_string, int radix);*/
// Инициализировать значение BN заданным целым числом
int bn_init_int(bn *t, int init_int){
    t->bodysize = 1;
    if(init_int < 0){
        t->sign = 1;
        init_int*=-1;
    }
    else
        t->sign = 0;
    t->body[0] = init_int;
    return 0;
}
// Уничтожить B (освободить память)
int bn_delete(bn *t){
    free(t->body);
    free(t);
    return 0;
}
// Операции, аналогичные +=, -=, *= /=, %=
int bn_add_to(bn *t, bn const *right){
    if(t->sign != right->sign){
        if(cmp(t,right) == 1) {
            int carry = 0;
            for(int i = 0; i < t->bodysize || carry; i++){
                if(right->bodysize > i)
                    t->body[i] = t->body[i] - right->body[i] - carry;
                else
                    t->body[i] = t->body[i] - carry;
                carry = (t->body[i] < 0);
                if (carry) {
                    t->body[i] += BASE;
                }
            }
        return 0;
        }
        if(cmp(t,right) == 0) {
            t->sign = (t->sign+1) % 2;
            int carry = 0;
            if (right->bodysize > t->bodysize) {
                t->body = realloc(t->body, sizeof(int) * right->bodysize);
                for (int i = t->bodysize; i < right->bodysize; i++)
                    t->body[i] = 0;
                t->bodysize = right->bodysize;
            }
            for(int i = 0; i < t->bodysize; i++) {
                t->body[i] = right->body[i] - t->body[i] - carry;
                carry = (t->body[i] < 0);
                if (carry) {
                    t->body[i] += BASE;
                }
            }
            return 0;
        }
    }
    else {
        int carry = 0;
        if (right->bodysize > t->bodysize) {
            t->body = realloc(t->body, sizeof(int) * right->bodysize);
            for (int i = t->bodysize; i < right->bodysize; i++)
                t->body[i] = 0;
            t->bodysize = right->bodysize;
        }
        for (int i = 0; i < t->bodysize || carry; i++) {
            if(right->bodysize > i)
                t->body[i] = t->body[i] + carry + right->body[i];
            else
                t->body[i] = t->body[i] + carry;
            carry = (t->body[i] >= BASE);
            if (carry) {
                t->body[i] -= BASE;
                if (t->bodysize <= i) {
                    t->body = realloc(t->body, sizeof(int) * t->bodysize * 2);
                    for (int j = t->bodysize; j < t->bodysize * 2; j++)
                        t->body[j] = 0;
                    t->bodysize = t->bodysize * 2;
                }
            }
        }
    return 0;
    }
    return -1;
}
int bn_sub_to(bn *t, bn const *right) {
    if(t->sign != right->sign) {
        int carry = 0;
        if (right->bodysize > t->bodysize) {
            t->body = realloc(t->body, sizeof(int) * right->bodysize);
            for (int i = t->bodysize; i < right->bodysize; i++)
                t->body[i] = 0;
            t->bodysize = right->bodysize;
        }
        for (int i = 0; i < t->bodysize || carry; i++) {
            if(right->bodysize > i)
                t->body[i] = t->body[i] + carry + right->body[i];
            else
                t->body[i] = t->body[i] + carry;
            carry = (t->body[i] >= BASE);
            if (carry) {
                t->body[i] -= BASE;
                if (t->bodysize <= i) {
                    t->body = realloc(t->body, sizeof(int) * t->bodysize * 2);
                    for (int j = t->bodysize; j < t->bodysize * 2; j++)
                        t->body[j] = 0;
                    t->bodysize = t->bodysize * 2;
                }
            }
        }
        return 0;
    }else {
        if(t->sign == right->sign){
            if(cmp(t,right) == 1) {
                int carry = 0;
                for(int i = 0; i < t->bodysize; i++){
                    t->body[i] -= right->body[i];
                    carry = (t->body[i] < 0);
                    if (carry) {
                        t->body[i] += BASE;
                        t->body[i+1] -= carry;
                    }
                }
                return 0;
            }
            if(cmp(t,right) == 0) {
                t->sign = (t->sign+1) % 2;
                int carry = 0;
                if (right->bodysize > t->bodysize) {
                    t->body = realloc(t->body, sizeof(int) * right->bodysize);
                    for (int i = t->bodysize; i < right->bodysize; i++)
                        t->body[i] = 0;
                    t->bodysize = right->bodysize;
                }
                for(int i = 0; i < t->bodysize; i++) {
                    t->body[i] = right->body[i] - t->body[i];
                    carry = (t->body[i] < 0);
                    if (carry) {
                        t->body[i] += BASE;
                        t->body[i+1] += carry;
                    }
                }
                return 0;
            }
        }
    }
    return -1;
}
int bn_mul_to(bn *t, bn const *right);
int bn_div_to(bn *t, bn const *right);
int bn_mod_to(bn *t, bn const *right);
//Вывод на экран
void bn_print(bn *t){
    if(t->sign == 1)
        printf("-");
    int size = t->bodysize;
    while(t->body[size-1] == 0 && size > 0)
        size--;
    if(size < 1)
        size = 1;
    printf ("%d", t->body[size-1]);
    for (int i = size - 2; i >= 0; i--)
        printf ("%09d", t->body[i]);
}

int main(){
    bn *f1 = bn_new();
    bn *f2 = bn_new();
    //bn *f3 = bn_new();
    //bn_init_string(f2,"-100000000000000000000000000000");
    //bn_init_string(f1,"-300000000000000000000000000000");
   /* bn_init_int(f1, 1);
    bn_init_int(f2, 1);
    int n;
    scanf("%d",&n);
    for(int i = 3; i <= n; i++){
        bn_delete(f3);
        f3 = bn_init(f2);
        bn_add_to(f2,f1);
        bn_delete(f1);
        f1 = bn_init(f3);
    }*/
    char *str1, *str2;
    str1 = (char*) malloc(sizeof(char) * 100001);
    str2 = (char*) malloc(sizeof(char) * 100001);
    char s;
    scanf("%s\n%c\n%s",str1,&s,str2);
    //printf("%s\n%c\n%s\n",str1,s,str2);
    bn_init_string(f1,str1);
    bn_init_string(f2,str2);
    free(str1);
    free(str2);
    //bn_print(f1);
    //printf("\n");
    //bn_print(f2);
    //printf("\n%d\n%d\n%d\n%d\n", f1->body[0], f1->body[1], f1->body[2], f1->body[3]);
    if(s == '+')
       bn_add_to(f1,f2);
    if(s == '-')
      bn_sub_to(f1,f2);
    bn_print(f1);
    //printf("\n%d\n%d\n%d\n%d", f1->body[0], f1->body[1], f1->body[2], f1->body[3]);
    //printf("%d\n %d\n %d\n", cmp(f2,f1), f2->bodysize, f1->bodysize);
    //bn_sub_to(f2,f1);
    //printf("%d\n",f2->body[0]);
    bn_delete(f1);
    bn_delete(f2);
    //bn_delete(f3);
    return 0;
}