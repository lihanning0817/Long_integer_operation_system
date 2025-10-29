// bigint2bin.h 头文件：进制转换相关函数原型声明
// 提供将十进制表示的 BigInt 转换为二进制/十六进制字符串的接口
#ifndef BIGINT2BIN_H
#define BIGINT2BIN_H

#include <stddef.h>
#include "bigint.h"

// 将十进制 BigInt 转换为二进制字符串，结果写入 out（包含可选负号），outlen 为缓冲区长度
// 输出以'\0'结尾。若缓冲区不足，输出为空字符串。
void bigint_to_binary(const BigInt *src, char *out, size_t outlen);

// 将十进制 BigInt 转换为十六进制字符串，结果写入 out（包含可选负号），outlen 为缓冲区长度
// 输出以'\0'结尾。若缓冲区不足，输出为空字符串。
void bigint_to_hex(const BigInt *src, char *out, size_t outlen);

#endif // BIGINT2BIN_H
