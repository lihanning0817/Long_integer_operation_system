//bigint.c源文件，包括长整数处理的相关函数的实现；
//实现：长整数的输入、输出、加减除运算等
#include "bigint.h"

// 开辟新结点
LinkList create_LNode(int d) {
	LinkList p = (LinkList)malloc(sizeof(LNode));
	if (!p) {
		printf("内存分配失败\n");
		exit(EXIT_FAILURE);
	}
	p->digit = d;
	p->next = NULL;
	return p;
}

// 插入高位，头插法
void insertHead(BigInt* num, int d) {
	LinkList p = create_LNode(d);
	p->next = num->head->next;
	num->head->next = p;
	num->length++;
}

// 插入低位，尾插法
void insertTail(BigInt* num, int d) {
	LinkList q = create_LNode(d);
	LinkList p = num->head;
	while (p->next != NULL) {
		p = p->next;
	}
    q->next=NULL;
	p->next = q;
	num->length++;
}

//比较绝对值大小，返回1表示a>b，0表示a==b，-1表示a<b
static int compareAbs(BigInt *a, BigInt *b) {
    if (a->length != b->length) return (a->length > b->length) ? 1 : -1;
    LinkList pa = a->head->next;
    LinkList pb = b->head->next;
    while (pa && pb) {
        if (pa->digit != pb->digit) return (pa->digit > pb->digit) ? 1 : -1;
        pa = pa->next;
        pb = pb->next;
    }
    return 0; // 相等
}

int compareBigInt(BigInt *a, BigInt *b) {
    /* 先看符号 */
    if (a->sign != b->sign) return (a->sign > b->sign) ? 1 : -1;
    /* 符号相同，比较绝对值 */
    int r = compareAbs(a, b);
    return (a->sign > 0) ? r : -r;
}


//反转链表表示的数字
static void reverseDigits(BigInt *num) {
    LinkList prev = NULL, cur = num->head->next;
    while (cur) {
        LinkList nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }
    num->head->next = prev;
}

//复制长整数
static void copyBigInt(const BigInt *src, BigInt *dest) {
    clearBigInt(dest);
    dest->sign = src->sign;
    for (LinkList p = src->head->next; p; p = p->next) {
        insertTail(dest, p->digit);
    }
}

//去掉前导零
static void removeLeadingZeros(BigInt *num) {
    if (!num || !num->head) return;
    while (num->length > 1 && num->head->next && num->head->next->digit == 0) {
        LinkList tmp = num->head->next;
        num->head->next = tmp->next;
        free(tmp);
        num->length--;
    }
    /* 0 的符号统一为正 */
    if (num->length == 1 && num->head->next && num->head->next->digit == 0) {
        num->sign = 1;
    }
}

/* res = |a| OP |b|，op=1 表示加法；op=-1 表示减法且要求 |a|>=|b| */
static void absCalc(const BigInt *a, const BigInt *b, int op, BigInt *res) {
    clearBigInt(res);

    BigInt A, B;
    InitBigInt(&A); 
    InitBigInt(&B);
    copyBigInt(a, &A);
    copyBigInt(b, &B);

    /* 反转后，从低位到高位逐位做加/减，便于进位/借位 */
    reverseDigits(&A);
    reverseDigits(&B);

    LinkList pa = A.head->next, pb = B.head->next;
    int carry = 0, borrow = 0;

    while (pa || pb) {
        int da = pa ? pa->digit : 0;
        int db = pb ? pb->digit : 0;
        int digit;

        if (op == 1) {                 /* 加 */
            int sum = da + db + carry;
            digit = sum % 10;
            carry = sum / 10;
        } else {                        /* 减：|a|-|b|，保证 |a|>=|b| */
            da -= borrow;
            if (da < db) { da += 10; borrow = 1; }
            else borrow = 0;
            digit = da - db;
        }
        insertHead(res, digit);
        if (pa) pa = pa->next;
        if (pb) pb = pb->next;
    }

    if (op == 1 && carry) insertHead(res, carry);
    removeLeadingZeros(res);

    freeBigInt(&A); freeBigInt(&B);
}


void addBigInt(BigInt *a, BigInt *b, BigInt *result) {
    clearBigInt(result);

    if (a->sign == b->sign) {
        /* 同号：|a|+|b|，结果符号同 a/b */
        absCalc(a, b, 1, result);
        result->sign = a->sign;
    } else {
        /* 异号：等价于 |较大|-|较小|，符号取绝对值较大的那个 */
        int cmp = compareAbs(a, b);
        if (cmp == 0) {
            insertTail(result, 0);
            result->sign = 1;
        } else if (cmp > 0) {
            absCalc(a, b, -1, result);
            result->sign = a->sign;
        } else {
            absCalc(b, a, -1, result);
            result->sign = b->sign;
        }
    }
    removeLeadingZeros(result);
}

void subBigInt(BigInt *a, BigInt *b, BigInt *result) {
    /* a-b = a + (-b) */
    BigInt nb; InitBigInt(&nb);
    copyBigInt(b, &nb);
    nb.sign = -nb.sign;
    addBigInt(a, &nb, result);
    freeBigInt(&nb);
}

// 辅助函数：计算 |a| * d，d 为单个数字（0-9）
static void mulAbsByInt(const BigInt *a, int d, BigInt *result) {
    clearBigInt(result);
    if (d == 0) {
        insertTail(result, 0);
        return;
    }
    BigInt A;
    InitBigInt(&A);
    copyBigInt(a, &A);
    reverseDigits(&A);

    int carry = 0;
    for (LinkList p = A.head->next; p; p = p->next) {
        int prod = p->digit * d + carry;
        insertHead(result, prod % 10);
        carry = prod / 10;
    }
    while (carry) {
        insertHead(result, carry % 10);
        carry /= 10;
    }
    removeLeadingZeros(result);
    freeBigInt(&A);
}   

// 长整数除法：a / b，结果存入 quotient 和 remainder
void divBigInt(BigInt *a, BigInt b, BigInt *quotient, BigInt *remainder){
    clearBigInt(quotient);
    clearBigInt(remainder);

    removeLeadingZeros(&b);

    // 除数为 0
    if (b.length == 1 && b.head->next && b.head->next->digit == 0) {
        printf("错误：除数不能为 0！\n");
        insertTail(quotient, 0);
        insertTail(remainder, 0);
        quotient->sign = remainder->sign = 1;
        return;
    }

    // 处理符号：计算时先按绝对值算，最后再确定符号
    int resultSign = a->sign * b.sign;

    BigInt Aabs, Babs;
    InitBigInt(&Aabs);
    InitBigInt(&Babs);
    copyBigInt(a, &Aabs);
    copyBigInt(&b, &Babs);
    Aabs.sign = Babs.sign = 1;
    removeLeadingZeros(&Aabs);
    removeLeadingZeros(&Babs);

    // 竖式除法：逐位下落 remainder，并确定每一位的商
    for (LinkList pa = Aabs.head->next; pa; pa = pa->next) {
        insertTail(remainder, pa->digit);
        removeLeadingZeros(remainder);

        int d = 0;
        if (compareAbs(remainder, &Babs) >= 0) {
            int lo = 1, hi = 9;
            while (lo <= hi) {
                int mid = (lo + hi) / 2;
                BigInt mul, tmp;
                InitBigInt(&mul);
                InitBigInt(&tmp);

                // tmp = |Babs| * mid
                mulAbsByInt(&Babs, mid, &mul);

                int cmp = compareAbs(remainder, &mul);
                if (cmp >= 0) {
                    d = mid;
                    lo = mid + 1;
                } else {
                    hi = mid - 1;
                }
                freeBigInt(&mul);
                freeBigInt(&tmp);
            }

            // remainder = remainder - Babs * d
            BigInt mul;
            InitBigInt(&mul);
            mulAbsByInt(&Babs, d, &mul);

            BigInt newR;
            InitBigInt(&newR);
            absCalc(remainder, &mul, -1, &newR);

            freeBigInt(remainder);
            *remainder = newR;
            freeBigInt(&mul);
        }

        insertTail(quotient, d);
    }

    removeLeadingZeros(quotient);
    removeLeadingZeros(remainder);

    quotient->sign = (quotient->length == 1 && quotient->head->next->digit == 0) ? 1 : resultSign;
    remainder->sign = a->sign;

    freeBigInt(&Aabs);
    freeBigInt(&Babs);
}



// 初始化长整数
void InitBigInt(BigInt* num) {
	num->head = create_LNode(-1); // 头节点（不存储有效数字）
	num->sign = 1;
	num->length = 0;
}

// 清空长整数（保留头结点，仅释放数据节点）
void clearBigInt(BigInt* num) {
	if (num == NULL || num->head == NULL) return;
	LinkList p = num->head->next;
	while (p) {
		LinkList tmp = p;
		p = p->next;
		free(tmp);
	}
	num->head->next = NULL;
	num->length = 0;
	num->sign = 1;
}


// 从输入读取长整数
void readBigInt(BigInt* num) {
	if (num == NULL) return;
	clearBigInt(num); // 先清空原有数据
	char str[101];
	if (scanf("%100s", str) != 1) return;
    int len = (int)strlen(str);
	int start = 0;

	// 处理符号位
	if (str[0] == '-') {
		num->sign = -1;
		start = 1;
	} else {
		num->sign = 1;
		start = 0;
	}

	// 从高位到低位插入（字符串左侧是高位）
	for (int i = start; i < len; i++) {
		if (str[i] < '0' || str[i] > '9') {
			printf("输入格式错误\n");
			clearBigInt(num);
			return;
		}
		insertTail(num, str[i] - '0');
	}

	// 处理全零情况（如"0000"）
	LinkList p = num->head->next;
	while (p && p->digit == 0 && num->length > 1) {
		LinkList temp = p;
		p = p->next;
		num->head->next = p;
		free(temp);
		num->length--;
	}

    removeLeadingZeros(num);

}

// 从高位到低位输出长整数
void printbigint(BigInt num) {
	if (num.length == 0) { // 空数
		printf("0\n");
		return;
	}
	if (num.sign == -1) {
		printf("-");
	}
	LinkList p = num.head->next;
	while (p) {
		printf("%d", p->digit);
		p = p->next;
	}
	printf("\n");
}

// 释放长整数链表
void freeBigInt(BigInt *num) {
    if (num == NULL || num->head == NULL) return;

    LinkList p = num->head;
    while (p != NULL) {
        LinkList temp = p;
        p = p->next;
        free(temp);
    }

    num->head = NULL;
    num->length = 0;
}
