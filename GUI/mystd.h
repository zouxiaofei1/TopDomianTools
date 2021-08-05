#pragma once
//×Ô¶¨ÒåµÄÓÃÓÚ¼õÉÙ³ÌÐòÌå»ýµÄ×Ö·û´®º¯Êý
//ÉÐ´¦ÓÚ²âÊÔ½×¶Î£¬²»Ò»¶¨ÎÈ¶¨
//
//by zouxiaofei1 2015 - 2021
//

unsigned int mywcslen(const wchar_t* wstr)
{
	int i = 0;
	if (wstr == 0)return 0;
	while (*wstr != L'\0')
	{
		++i;
		++wstr;
	}
	return i;
}
void myitoa(unsigned long val, char* buf, unsigned radix)
{
	char* p;/*pointer to traverse string*/
	char* firstdig;/*pointer to first digit*/
	char temp;/*temp char*/
	unsigned digval;/*value of digit*/

	p = buf;
	firstdig = p;/*save pointer to first digit*/

	do {
		digval = (unsigned)(val % radix);
		val /= radix;/*get next digit*/

		/*convert to ascii and store */
		if (digval > 9)
			*p++ = (char)(digval - 10 + 'a ');/*a letter*/
		else
			*p++ = (char)(digval + '0 ');/*a digit*/
	} while (val > 0);

	/*We now have the digit of the number in the buffer,but in reverse
	order.Thus we reverse them now.*/

	*p-- = '\0 ';/*terminate string;p points to last digit*/

	do {
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;/*swap *p and *firstdig*/
		--p;
		++firstdig;/*advance to next two digits*/
	} while (firstdig < p);/*repeat until halfway*/
}
int myatoi(char* str) {
	int data = 0, i = 0;
	int sign = 1;

	/* 略过空白 */
	while (*str == ' ') {
		str++;
	}

	if (*str == '-' || *str == '+') {
		sign = 1 - 2 * (*str++ == '-');/* 耍了个小聪明，也可以分开写 */
	}

	while ((*str >= '0') && (*str <= '9')) {
		/* 每次取得新值前检测是否在[0，2,147,483,647]之间，防止溢出 */
		if (data > INT_MAX / 10 || (data == INT_MAX / 10 && (str[i] > '7'))) {/* 7为常量值的最后一位数字，正负均满足要求 */
			if (sign == -1) {
				return INT_MIN;
			}
			else {
				return INT_MAX;
			}
		}

		data = data * 10 + (*str++ - '0');
	}

	return data * sign;
}

char* mystrrchr(const char* str, int ch)
{
	char* start = (char*)str;
	while (*str++)/*get the end of the string*/
		;
	while (--str != start && *str != (char)ch)
		;
	if (*str == (char)ch)
		return((char*)str);
	return NULL;

}

char* mystrcat(char* dst, const char* src)
{
	char* temp = dst;
	while (*temp != '\0')
		temp++;
	while ((*temp++ = *src++) != '\0');

	return dst;
}

char* mystrcpy(char* _Dest, const char* _Source)
{
	if (NULL == _Dest || NULL == _Source)
		return NULL;
	char* ret = _Dest;
	int i = 0;
	for (i = 0; _Source[i] != '\0'; i++)
	{
		_Dest[i] = _Source[i];
	}
	_Dest[i] = '\0';
	return ret;
}

int mystrlen(const char* StrDest)
{
	int i;
	i = 0;
	while ((*StrDest++) != '\0')
	{
		i++;
	}//这个循环体意思是从字符串第一个字符起计数，只遇到字符串结束标志'\0’才停止计数
	return i;
}

wchar_t* mywcscpy(wchar_t* dest, const wchar_t* source)
{
	if (NULL == dest || NULL == source)
	{
		return NULL;
	}
	else
	{
		wchar_t* p = dest;
		while (*dest++ = *source++)
		{
		}
		return p;
	}
}

wchar_t* myscpy(wchar_t* dest, const wchar_t* source)
{
	if (NULL == dest || NULL == source)
	{
		return NULL;
	}
	else
	{
		wchar_t* p = dest;
		while (*dest++ = *source++)
		{
		}
		return p;
	}
}
int mywcscmp(const wchar_t* src, const wchar_t* dst)//weak
{
	//while (!(ret = *(wchar_t*)src - *(wchar_t*)dst) && *dst)
	/*while (!(ret = *(wchar_t*)src - *(wchar_t*)dst) && *dst)
	{
		++src;
		++dst;
	}
	if (ret < 0) ret = -1;
	else if (ret > 0) ret = 1;
	return ret;*/
	while ((*src == *dst) && *dst)++src, ++dst;
	if (*src == *dst)return 0; else return 1;
}

__forceinline bool isSign(wchar_t c)
{
	return (c == L'+' || c == L'-');
}

__forceinline bool isDigit(wchar_t c)
{
	return (c >= L'0' && c <= L'9');
}

bool miniwcsstr(wchar_t chr, const wchar_t* str) {
	while (*str) {
		if (chr == *str) return true;
		str++;
	}
	return false;
}

const wchar_t* mywcschr(const wchar_t* str, wchar_t c) {
	for (; *str != 0; ++str) {
		if (*str == c) {
			return str;
		}
	}
	return NULL;
}
wchar_t* mywcstok(wchar_t* s, const wchar_t* delim) {
	static wchar_t* text = NULL;
	if (text == NULL) text = s;
	if (text == NULL) return NULL;
	wchar_t* head = text;
	while (*text && !miniwcsstr(*text, delim)) {
		text++;
	}
	while (*text && miniwcsstr(*text, delim)) {
		*text = '\0';
		text++;
	}
	if (*text == '\0') text = NULL;
	return head;
}

int mywtoi(wchar_t* str) {
	int idx = 0, rst = 0;
	long sign = 0, num = 0;

	if (NULL != str)
	{
		while (str[idx])
		{
			if ((num * sign) >= INT_MAX || (num * sign) <= INT_MIN)
			{
				break;
			}

			if (isDigit(str[idx]))
			{
				num = num * 10 + str[idx] - L'0';
				sign = sign == 0 ? 1 : sign;
			}
			else
			{
				if (num == 0 && sign == 0)
				{
					switch (str[idx])
					{
					case L'+': sign = 1;   break;
					case L'-': sign = -1;  break;
					case L' ': break;                        //skip space
					default:  idx = mywcslen(str) - 1; break;   //stop and return 0
					}
				}
				else
					break;
			}

			idx++;
		}
	}

	rst = (num * sign >= INT_MAX) ? INT_MAX : (num * sign <= INT_MIN ? INT_MIN : (int)(num & 0x000000007FFFFFFF) * sign);

	return rst;
}
wchar_t* mywcsrchr(wchar_t* str, wchar_t ch)//const±£»¤Ô´×Ö·û´®
{
	int len = mywcslen(str);
	wchar_t* last = str + len - 1;//¶¨ÒåÒ»¸ö±äÁ¿last£¬Ö¸ÏòstrµÄ×îºóÒ»¸ö×Ö·û

	while (last != str)
	{
		if (*last == ch)
			return last;
		last--;
	}
	return NULL;//Èç¹ûÃ»ÓÐÕÒµ½£¬º¯Êý·µ»ØÒ»¸öNULLÖ¸Õë
}
void* mymemcpy(void* dest, const void* src, size_t n)
{
	if (NULL == dest || NULL == src || n < 0)
		return NULL;
	char* tempDest = (char*)dest;
	char* tempSrc = (char*)src;

	while (n-- > 0)
		*tempDest++ = *tempSrc++;
	return dest;
}
#ifndef _WIN64
void myZeroMemory(void* source,  size_t dwsize)
{
	__asm
	{
		mov ecx, dwsize
		//ecx: Ñ­»·´ÎÊý
		xor eax, eax
		mov edi, [source]
		rep stos[edi]
	}
}

#else
//#define myZeroMemory ZeroMemory

void myZeroMemory(wchar_t* src, size_t count)
{
	for (int i = 0; i < count / sizeof(wchar_t); ++i)src[i] = 0;
}
void myZeroMemory(void* src, size_t count)
{
	size_t t = count /2;
	char* tmpsrc = (char*)src;
	while (t--)
		*tmpsrc++ = 0;
}


#endif // _WIN64



wchar_t* __cdecl mywcsstr(
	const wchar_t* wcs1,
	const wchar_t* wcs2
)
{
	wchar_t* cp = (wchar_t*)wcs1;
	wchar_t* s1, * s2;

	if (!*wcs2)
		return (wchar_t*)wcs1;

	while (*cp)
	{
		s1 = cp;
		s2 = (wchar_t*)wcs2;

		while (*s1 && *s2 && !(*s1 - *s2))
			++s1, ++s2;

		if (!*s2)
			return(cp);

		cp++;
	}

	return 0;
}
void mywcslwr(wchar_t* src)
{
	while (*src != 0)
	{
		if (*src > 'A' && *src <= 'Z') {
			//*src += 0x20; 
			*src += 32;
		}
		src++;
	}
}

void mywcscat(wchar_t* dst, const wchar_t* src)
{
	wchar_t* temp = dst;
	while (*temp !=0)
		temp++;
	while ((*temp++ = *src++) !=0);

}

void myitow(int num, wchar_t* str, int radix)
{
	(radix);
	/* Ë÷Òý±í */
	wchar_t index[] = L"0123456789";
	unsigned unum; /* ÖÐ¼ä±äÁ¿ */
	int i = 0, j, k;
	/* È·¶¨unumµÄÖµ */
	if (num < 0) /* Ê®½øÖÆ¸ºÊý */
	{
		unum = (unsigned)-num;
		str[i++] = L'-';
	}
	else unum = (unsigned)num; /* ÆäËüÇé¿ö */
	/* ÄæÐò */
	do
	{
		str[i++] = index[unum % 10];
		unum /= 10;
	} while (unum);
	str[i] = 0;
	/* ×ª»» */
	if (str[0] == L'-') k = 1; /* Ê®½øÖÆ¸ºÊý */
	else k = 0;
	/* ½«Ô­À´µÄ¡°/2¡±¸ÄÎª¡°/2.0¡±£¬±£Ö¤µ±numÔÚ16~255Ö®¼ä£¬radixµÈÓÚ16Ê±£¬Ò²ÄÜµÃµ½ÕýÈ·½á¹û */
	wchar_t temp;
	for (j = k; j <= (i - 1) / (float)2.0; j++)
	{

		temp = str[j];
		str[j] = str[i - j - 1 + k];
		str[i - j - 1 + k] = temp;// s(str[j]);
	}
}