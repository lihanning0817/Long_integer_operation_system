#include <stdio.h>
#include "bigint.h"
#include "bigint2bin.h"

void printMenu() {
    printf("\n============================\n");
    printf("       长整数计算器\n");
    printf("============================\n");
    printf("1. 输入长整数 A\n");
    printf("2. 输入长整数 B\n");
    printf("3. 输出 A 和 B\n");
    printf("4. 计算 A + B\n");
    printf("5. 计算 A - B\n");
    printf("6. 计算 A / B (得到商和余数)\n");
    printf("0. 退出系统\n");
    printf("7. 进制转换（将 A 或 B 转为二进制/十六进制）\n");
    printf("============================\n");
    printf("请输入你的选择：");
}

int main() {
    BigInt A, B, R, Q, Rem;
    InitBigInt(&A);
    InitBigInt(&B);
    InitBigInt(&R);
    InitBigInt(&Q);
    InitBigInt(&Rem);

    int choice;

    while (1) {
        printMenu();
        scanf("%d", &choice);

        if (choice == 0) {
            printf("已退出程序。\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("请输入 A：");
                readBigInt(&A);
                printf("A = "); printbigint(A);
                break;

            case 2:
                printf("请输入 B：");
                readBigInt(&B);
                printf("B = "); printbigint(B);
                break;

            case 3:
                printf("A = "); printbigint(A);
                printf("B = "); printbigint(B);
                break;

            case 4:
                addBigInt(&A, &B, &R);
                printf("A = "); printbigint(A);
                printf("B = "); printbigint(B);
                printf("A + B = "); printbigint(R);
                break;

            case 5:
                subBigInt(&A, &B, &R);
                printf("A = "); printbigint(A);
                printf("B = "); printbigint(B);
                printf("A - B = "); printbigint(R);
                break;

            case 6:
                // 初始化商和余数
                clearBigInt(&Q);
                clearBigInt(&Rem);

                divBigInt(&A, B, &Q, &Rem);
                printf("A = "); printbigint(A);
                printf("B = "); printbigint(B);

                printf("商  = "); printbigint(Q);
                printf("余数 = "); printbigint(Rem);
                break;

            case 7: {
                printf("选择要转换的数（1=A, 2=B）：");
                int sel = 0;
                if (scanf("%d", &sel) != 1) { fflush(stdin); break; }
                BigInt *target = (sel == 2) ? &B : &A;
                if (target->head == NULL || target->length == 0) {
                    printf("目标数为空，请先输入对应的数。\n");
                    break;
                }
                char buf[4096];
                bigint_to_binary(target, buf, sizeof(buf));
                if (buf[0] == '\0') printf("二进制转换失败（缓冲区不足）\n");
                else printf("二进制：%s\n", buf);

                bigint_to_hex(target, buf, sizeof(buf));
                if (buf[0] == '\0') printf("十六进制转换失败（缓冲区不足）\n");
                else printf("十六进制：%s\n", buf);
                break;
            }

            default:
                printf("输入错误，请重新选择！\n");
                break;
        }
    }

    // 释放内存
    freeBigInt(&A);
    freeBigInt(&B);
    freeBigInt(&R);
    freeBigInt(&Q);
    freeBigInt(&Rem);

    return 0;
}
