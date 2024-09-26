#include <stdlib.h>

#include "uthash.h"

#define ANS_SIZE 2

// 双循环，暴力解法
int* twoSum_brute_force(int* nums, int numsSize, int target, int* returnSize) {
    for (int i = 0; i < numsSize; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            if (nums[i] + nums[j] == target) {
                int* ans = (int*)malloc(ANS_SIZE * sizeof(int));
                ans[0] = i;
                ans[1] = j;
                *returnSize = ANS_SIZE;
                return ans;
            }
        }
    }

    *returnSize = 0;
    return NULL;
}

/**
 * 使用哈希表
 * 注意：先判断，再添加
 */

struct hashTable {
    int key;  // use this field as the key
    int val;
    UT_hash_handle hh;  // make this structure hashtable
};

struct hashTable* hashtable;

struct hashTable* find(int key) {
    struct hashTable* tmp;
    HASH_FIND_INT(hashtable, &key, tmp);  // 查找 key
    return tmp;                           // 返回结果
}

void insert(int ikey, int ival) {
    struct hashTable* it = find(ikey);  // 查找 ikey
    if (it == NULL) {                   // 不存在则新增
        struct hashTable* tmp = malloc(sizeof(struct hashTable));
        tmp->key = ikey;
        tmp->val = ival;
        HASH_ADD_INT(hashtable, key, tmp);
    }
    else {  // 存在则更新
        it->val = ival;
    }
}

void free_hashtable() {
    struct hashTable* cur, * tmp;

    HASH_ITER(hh, hashtable, cur, tmp) {
        HASH_DEL(hashtable, cur);
        free(cur); // 释放内存
    }

    HASH_CLEAR(hh, hashtable); // 重置哈希表本身
}

int* twoSum_hashtable(int* nums, int numsSize, int target, int* returnSize) {
    hashtable = NULL;
    for (int i = 0; i < numsSize; i++) {
        struct hashTable* it = find(target - nums[i]);
        if (it != NULL) {
            int* ans = malloc(ANS_SIZE * sizeof(int));
            ans[0] = i;
            ans[1] = it->val;
            *returnSize = ANS_SIZE;

            free_hashtable();
            return ans;
        }

        insert(nums[i], i);
    }

    free_hashtable();
    *returnSize = 0;
    return NULL;
}