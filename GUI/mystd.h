#pragma once
//�Զ�������ڼ��ٳ���������ַ�������
//�д��ڲ��Խ׶Σ���һ���ȶ�
//
//by zouxiaofei1 2015 - 2020
//

int statu = GetTickCount();
int myrand() {
	statu = 214013 * statu + 2531011;
	return statu >> 16 & ((1 << 15) - 1);
}

unsigned int mywcslen(const wchar_t* wstr)
{
	int i = 0;
	while (*wstr != L'\0')
	{
		++i;
		++wstr;
	}
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
int mywcscmp(const wchar_t* src, const wchar_t* dst)
{
	int ret = 0;
	while (!(ret = *(wchar_t*)src - *(wchar_t*)dst) && *dst)
	{
		++src;
		++dst;
	}
	if (ret < 0) ret = -1;
	else if (ret > 0) ret = 1;
	return ret;
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
wchar_t* mywcsrchr(wchar_t* str, int ch)//const����Դ�ַ���
{
	int len = mywcslen(str);
	wchar_t* last = str + len - 1;//����һ������last��ָ��str�����һ���ַ�

	while (*last != ch)
	{
		if (*last == ch)
			return last;
		last--;
	}
	return NULL;//���û���ҵ�����������һ��NULLָ��
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

#undef ZeroMemory

void ZeroMemory(void* s,  size_t n)
{
	if (NULL == s || n < 0)
		return;
	char* tmpS = (char*)s;
	while (n-- > 0)
		*tmpS++ = 0;
}
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

	return(NULL);
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
	/* ������ */
	wchar_t index[] = L"0123456789";
	unsigned unum; /* �м���� */
	int i = 0, j, k;
	/* ȷ��unum��ֵ */
	if (num < 0) /* ʮ���Ƹ��� */
	{
		unum = (unsigned)-num;
		str[i++] = L'-';
	}
	else unum = (unsigned)num; /* ������� */
	/* ���� */
	do
	{
		str[i++] = index[unum % 10];
		unum /= 10;
	} while (unum);
	str[i] = 0;
	/* ת�� */
	if (str[0] == L'-') k = 1; /* ʮ���Ƹ��� */
	else k = 0;
	/* ��ԭ���ġ�/2����Ϊ��/2.0������֤��num��16~255֮�䣬radix����16ʱ��Ҳ�ܵõ���ȷ��� */
	wchar_t temp;
	for (j = k; j <= (i - 1) / (float)2.0; j++)
	{

		temp = str[j];
		str[j] = str[i - j - 1 + k];
		str[i - j - 1 + k] = temp;// s(str[j]);
	}
}