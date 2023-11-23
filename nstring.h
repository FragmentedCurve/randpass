#ifndef _NSTRING_H_
#define _NSTRING_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

typedef struct string {
	size_t length;
	const char* s;
} string;

#define String(s)     ((string){sizeof(s) - 1, (s)})
#define StringC(s)    ((string){strlen(s), (s)})
#define StringN(s, n) ((string){(n), (char*)(s)})

#define $(s)      String(s)
#define $$(s)     StringC(s)
#define $$$(s, n) StringN(s, n)

string inline static
Slice(string s, size_t start, size_t end)
{
	return ((string){end - start, s.s + start});
}
#define Prefix(s, index) Slice((s), 0, (index))
#define Suffix(s, index) Slice((s), s.length - index, s.length)

void inline static
ToStrings(string* dest, char** strings, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		dest[i] = $$(strings[i]);
}

int inline static
Equals(string a, string b)
{
	if (a.length != b.length)
		return 0;

	return !strncmp(a.s, b.s, a.length);
}
#define Equals$(a, b) Equals($(a), b)

int inline static
HasPrefix(string prefix, string s)
{
	return Equals(prefix, Slice(s, 0, prefix.length));
}
#define HasPrefix$(p, s) HasPrefix($(p), s)

ssize_t inline static
IndexOf(string haystack, char needle)
{
	for (size_t i = 0; i < haystack.length; ++i)
		if (haystack.s[i] == needle)
			return i;
	return -1;
}
#define IndexOf$(haystack, needle) IndexOf($(haystack), needle)

string inline static
Itoa(size_t number, char buf[32], size_t len)
{
	assert(len >= 32);
	size_t length = 0;

	buf += sizeof(char) * 31;

	if (number == 0) {
		*buf = '0';
		return $$$(buf, 1);
	}

	while (number) {
		*buf = (number % 10) + '0';
		--buf;
		number /= 10;
		++length;
	}

	return $$$(buf+1, length);
}

static void PrintFd(int fd, string fmt, ...);

void inline static
_Print(int fd, string fmt, va_list args)
{
	size_t index = IndexOf(fmt, '%');
	if (index == -1) {
		write(fd, fmt.s, fmt.length);
		return;
	}

	// Print out everything until the '%'
	write(fd, fmt.s, index);

	// Handle the '%' formatter
	char nbuf[32];
	ssize_t param;
	switch (fmt.s[++index]) {
	case '%': { // Escape
		write(fd, "%", 1);
		++index;
	} break;
	case 's': { // String
		string s = va_arg(args, string);
		write(fd, s.s, s.length);
		++index;
	} break;
	case 'd': { // 32 Number
		param = va_arg(args, int);

		if (param < 0) {
			write(fd, "-", 1);
			param *= -1;
		}

		string s = Itoa((unsigned int)param, nbuf, sizeof nbuf);
		write(fd, s.s, s.length);
		++index;
	} break;
	case 'l': { // 64 Number
		param = va_arg(args, size_t);

		if (param < 0) {
			write(fd, "-", 1);
			param *= -1;
		}

		string s = Itoa((size_t)param, nbuf, sizeof nbuf);
		write(fd, s.s, s.length);
		++index;
	} break;
	case 'c': { // Character
		char c = (char) va_arg(args, int);
		write(fd, &c, 1);
		++index;
	} break;
	default: {
		// Bug-free code should never reach this.
		PrintFd(STDERR_FILENO, String("_Print: invalid formatter '%%%c'\n"), fmt.s[index]);
		assert(0);
	} break;
	}

	if (index > fmt.length)
		return;

	string t = Slice(fmt, index, fmt.length);
	_Print(fd, t, args);
}

void static
PrintFd(int fd, string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_Print(fd, fmt, args);
	va_end(args);
}
#define PrintFd$(fd, fmt, ...) PrintFd(fd, String(fmt), ##__VA_ARGS__)

#define PrintErr(fmt, ...) PrintFd(STDERR_FILENO, fmt, ##__VA_ARGS__)
#define PrintErr$(fmt, ...) PrintErr($(fmt), ##__VA_ARGS__)


/* Create a version of main called Main whose signature is Main(int, string*); */
#define Main(c, v)						\
	Main(c, v);						\
	int main(int argc, char** argv) {			\
		string* args = alloca(argc * sizeof(string));	\
		ToStrings(args, argv, argc);			\
		return Main(argc, args);			\
	}							\
	int Main(c, v)

#endif /* _NSTRING_H_ */
