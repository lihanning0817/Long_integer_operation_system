// bigint2bin.c
// 实现十进制 BigInt 到二进制/十六进制字符串的转换

#include "bigint2bin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 将 src 复制到 dest（深拷贝），dest 必须为未初始化状态
static void copyBigInt(const BigInt *src, BigInt *dest) {
    InitBigInt(dest);
    if (src == NULL || src->head == NULL) return;
    LinkList p = src->head->next;
    while (p) {
        insertTail(dest, p->digit);
        p = p->next;
    }
    dest->sign = src->sign;
}

// 对十进制大整数 src 执行除以 small 的操作，得到商写入 quotient（已初始化过，将被赋值），返回余数。
// small 应为正且 >= 2 且 <= 16。
static int divmod_small(const BigInt *src, int small, BigInt *quotient) {
    if (src == NULL || src->head == NULL) return 0;
    InitBigInt(quotient);
    LinkList p = src->head->next;
    int carry = 0;
    int started = 0; // 是否已经开始写入商的有效位
    while (p) {
        int cur = carry * 10 + p->digit;
        int qd = cur / small; // 商的当前十进制位（一定在 0..9）
        carry = cur % small;
        if (qd != 0 || started) {
            insertTail(quotient, qd);
            started = 1;
        }
        p = p->next;
    }
    // 如果商为 0，则至少写入一位 0
    if (!started) {
        insertTail(quotient, 0);
    }
    quotient->sign = 1; // 商为非负（我们按绝对值处理）
    return carry;
}

// helper: 判断 BigInt 是否为 0
static int is_zero(const BigInt *a) {
    if (a == NULL || a->head == NULL) return 1;
    return (a->length == 0) || (a->length == 1 && a->head->next && a->head->next->digit == 0);
}

// 通用转换函数：将 src（十进制）转为 base（2 或 16），把结果字符串写入 out（反向收集后再反转）
static void bigint_to_base_internal(const BigInt *src, int base, char *out, size_t outlen) {
    if (out == NULL || outlen == 0) return;
    out[0] = '\0';
    if (src == NULL || src->head == NULL) return;

    // 处理零
    if (is_zero(src)) {
        if (outlen >= 2) {
            out[0] = '0'; out[1] = '\0';
        }
        return;
    }

    // 工作副本（按绝对值计算），避免修改源对象
    BigInt work;
    copyBigInt(src, &work);
    work.sign = 1;

    // 临时缓冲来收集余数（反向）
    char tmp[8192];
    size_t ti = 0;

    while (!is_zero(&work) && ti + 2 < sizeof(tmp)) {
        BigInt q;
        int rem = divmod_small(&work, base, &q);
        // 将余数转换为字符：0-9 / A-F
    if (rem < 10) tmp[ti++] = (char)('0' + rem);
    else tmp[ti++] = (char)('A' + (rem - 10));

        // 释放旧 work 并用 q 替代
        freeBigInt(&work);
        work = q;
    }

    // 处理符号
    int need_sign = (src->sign < 0);

    // 计算所需长度（符号 + digits + null）
    size_t needed = ti + (need_sign ? 1 : 0) + 1;
    if (needed > outlen) {
        // 缓冲区不足：写空字符串作为错误信号
        freeBigInt(&work);
        out[0] = '\0';
        return;
    }

    size_t oi = 0;
    if (need_sign) out[oi++] = '-';

    // tmp 中是低位到高位，反转写入 out
    for (size_t i = 0; i < ti; i++) {
        out[oi + i] = tmp[ti - 1 - i];
    }
    oi += ti;
    out[oi] = '\0';

    freeBigInt(&work);
}

void bigint_to_binary(const BigInt *src, char *out, size_t outlen) {
    bigint_to_base_internal(src, 2, out, outlen);
}

void bigint_to_hex(const BigInt *src, char *out, size_t outlen) {
    bigint_to_base_internal(src, 16, out, outlen);
}
//bigint2bin.c源文件，实现进制转换的函数的实现
//实现十进制 → 二进制/十六进制转换
