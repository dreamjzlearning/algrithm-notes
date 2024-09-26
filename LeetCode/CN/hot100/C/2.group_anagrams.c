#include "uthash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------
// 简单动态字符串数组 
//---------------------------------------------------------------------

/**
* @brief 动态字符串数组
*
*/
typedef struct {
    char** data; // 字符串数组
    unsigned int len; // 长度，元素个数
    size_t cap; // 容量，实际分配内存空间大小
} StrArr;

/**
 * @brief 创建 StrArr 对象
 *
 * @return StrArr*
 */
StrArr* new_str_arr() {
    StrArr* tmp = malloc(sizeof(StrArr));
    tmp->data = NULL;
    tmp->len = 0;
    tmp->cap = 0;
    return tmp;
}

/**
 * @brief 将字符串追加至数组尾部
 *
 * @param sa 动态数组
 * @param s 待添加字符串
 */
void append_strarr(StrArr* sa, const char* s) {
    // 拷贝字符串
    char* tmp_str = malloc(sizeof(char) * (strlen(s) + 1));
    strcpy(tmp_str, s);

    if (sa->data == NULL) { // 数组为空则新建
        // creat new array
        sa->data = malloc(sizeof(char*));
        // add first element
        sa->data[0] = tmp_str;
        sa->cap = 1;
    }
    else if (sa->len + 1 > sa->cap) { // 容量不足
        sa->data = realloc(sa->data, 2 * sa->cap * sizeof(char*)); // 简单 2 倍扩容
        sa->data[sa->len] = tmp_str;
        sa->cap *= 2;
    }
    else {
        sa->data[sa->len] = tmp_str;
    }

    sa->len++;
}

/**
 * @brief 释放 StrArr 内存
 *
 * @param sa
 */
void free_strarr(StrArr* sa) {
    // 先释放所有的字符串
    for (int i = 0; i < sa->len; i++) {
        free(sa->data[i]);
    }

    // 释放自身
    free(sa);
}

/**
 * @brief 简单打印
 *
 * @param sa
 */
void print_strarr(const StrArr* sa) {
    printf("[ ");
    for (int i = 0; i < sa->len; i++) {
        printf("\"%s\", ", sa->data[i]);
    }
    printf("] Len: %d, Cap: %d\n", sa->len, sa->cap);
}

//---------------------------------------------------------------------
// 哈希表
// Key：字符串，Val：StrArr
//---------------------------------------------------------------------

/**
 * @brief 哈希表
 *
 */
typedef struct {
    char* key;
    StrArr* val;
    UT_hash_handle hh;
} HashTable;

HashTable* ht = NULL;

/**
 * @brief 查找
 *
 * @param key
 * @return HashTable*
 */
HashTable* find(const char* key) {
    HashTable* tmp;
    HASH_FIND_STR(ht, key, tmp);
    return tmp;
}

/**
 * @brief 添加元素
 *
 * @param key
 * @param val
 */
void add(const char* key, const char* val) {
    HashTable* s;
    HASH_FIND_STR(ht, key, s);
    if (s == NULL) { // 没有则新增
        s = malloc(sizeof(*s));
        s->key = malloc(sizeof(char) * (strlen(key) + 1));
        strcpy(s->key, key);
        s->val = new_str_arr();
        HASH_ADD_STR(ht, key, s);
    }
    append_strarr(s->val, val); // 已存在，则追加至数组中
}

/**
 * @brief 释放内存
 *
 */
void free_ht() {
    HashTable* cur, * tmp;
    // 遍历，释放所有key和val的内存
    // 然后从表中删除
    HASH_ITER(hh, ht, cur, tmp) {
        free(cur->key);
        free_strarr(cur->val);
        // delete from hashtable
        HASH_DEL(ht, cur);
    }
}

/**
 * @brief 简单打印
 *
 */
void print_ht() {
    HashTable* cur, * tmp;
    HASH_ITER(hh, ht, cur, tmp) {
        printf("\"%s\": ", cur->key);
        print_strarr(cur->val);
    }
}

//---------------------------------------------------------------------
// 方法一
// 异位词排序后的结果相同
// 将异位词排序结果作为 key, 字符串数组作为 val
//---------------------------------------------------------------------

int compare(const void* a, const void* b) {
    return *(char*)a - *(char*)b;
}

char*** groupAnagrams(char** strs, int strsSize, int* returnSize, int** returnColumnSizes) {
    // 边界条件，空字符串提前结束
    if (strsSize == 0) {
        *returnSize = 0;
        return NULL;
    }
    // 遍历数组，进行分组
    for (int i = 0; i < strsSize; i++) {
        char* cur_str = strs[i];
        char* sort_str = malloc(sizeof(char) * (strlen(cur_str) + 1));
        strcpy(sort_str, cur_str);
        qsort(sort_str, strlen(sort_str), sizeof(char), compare);
        add(sort_str, cur_str);
    }

    // 提取分组结果
    unsigned int entry_count = HASH_COUNT(ht);
    *returnSize = (int)entry_count;
    *returnColumnSizes = malloc(entry_count * sizeof(int));
    char*** ans = malloc(entry_count * sizeof(char**));

    HashTable* cur, * tmp;
    int row = 0;
    HASH_ITER(hh, ht, cur, tmp) {
        StrArr* sa = cur->val;
        unsigned int arr_len = sa->len;
        ans[row] = malloc(arr_len * sizeof(char*));
        (*returnColumnSizes)[row] = (int)arr_len;

        for (int col = 0; col < arr_len; col++) {
            char* cur_str = sa->data[col];
            unsigned int cur_str_size = strlen(cur_str) + 1;
            ans[row][col] = malloc(sizeof(char) * cur_str_size);
            strcpy(ans[row][col], cur_str);
        }
        row++;
    }

    free_ht(); // 释放内存
    ht = NULL; // 重置哈希表

    return ans;
}


//---------------------------------------------------------------------
// 方法二
// 异位词字母出现次数相同
// 以字母计数数组作为 Key，字符串数组作为 Val
//---------------------------------------------------------------------

#define LETTER_NUM 26 // 字母个数

/**
 * @brief 以结构体作为 Key
 *
 */
typedef struct {
    short count[LETTER_NUM];
} HKey;

typedef struct {
    HKey key;
    StrArr* val;
    UT_hash_handle hh;
} HashTable2;

HashTable2* ht2 = NULL;

HashTable2* find2(const short* c) {
    HKey* k = malloc(sizeof(HKey));
    memcpy(k->count, c, sizeof(short) * LETTER_NUM);

    HashTable2* tmp;
    HASH_FIND(hh, ht2, k, sizeof(HKey), tmp);
    return tmp;
}

void add2(const short* key, const char* val) {
    HKey* k = malloc(sizeof(HKey));
    memcpy(k->count, key, sizeof(short) * LETTER_NUM);

    HashTable2* s;
    HASH_FIND(hh, ht2, k, sizeof(HKey), s);

    if (s == NULL) {
        s = malloc(sizeof(*s));
        memcpy(s->key.count, key, sizeof(short) * LETTER_NUM);
        s->val = new_str_arr();
        HASH_ADD(hh, ht2, key, sizeof(HKey), s);
    }
    append_strarr(s->val, val);
}

void free_ht2() {
    HashTable2* cur, * tmp;
    // 遍历，释放所有key和val的内存
    // 然后从表中删除
    HASH_ITER(hh, ht2, cur, tmp) {
        free_strarr(cur->val);
        // delete from hashtable
        HASH_DEL(ht2, cur);
    }
}

/**
 * @brief 简单打印
 *
 */
void print_ht2() {
    HashTable2* cur, * tmp;
    HASH_ITER(hh, ht2, cur, tmp) {
        printf("{[");
        for (int i = 0; i < LETTER_NUM; i++) {
            printf("%d, ", cur->key.count[i]);
        }
        printf("]: ");
        print_strarr(cur->val);
    }
}

char*** groupAnagrams_2(char** strs, int strsSize, int* returnSize, int** returnColumnSizes) {
    if (strsSize == 0) {
        *returnSize = 0;
        return NULL;
    }

    for (int i = 0; i < strsSize; i++) {
        short c[LETTER_NUM] = { 0 };
        for (int j = 0; j < strlen(strs[i]); j++) {
            c[strs[i][j] - 'a']++;
        }
        add2(c, strs[i]);
    }

    // 提取分组结果
    unsigned int entry_count = HASH_COUNT(ht2);
    *returnSize = (int)entry_count;
    *returnColumnSizes = malloc(entry_count * sizeof(int));
    char*** ans = malloc(entry_count * sizeof(char**));

    HashTable2* cur, * tmp;
    int row = 0;
    HASH_ITER(hh, ht2, cur, tmp) {
        StrArr* sa = cur->val;
        unsigned int arr_len = sa->len;
        ans[row] = malloc(arr_len * sizeof(char*));
        (*returnColumnSizes)[row] = (int)arr_len;

        for (int col = 0; col < arr_len; col++) {
            char* cur_str = sa->data[col];
            unsigned int cur_str_size = strlen(cur_str) + 1;
            ans[row][col] = malloc(sizeof(char) * cur_str_size);
            strcpy(ans[row][col], cur_str);
        }
        row++;
    }

    free_ht2(); // 释放内存
    ht2 = NULL; // 重置哈希表

    return ans;

}