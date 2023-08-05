#include "s21_string.h"

// Выполняет поиск первого вхождения символа c (беззнаковый тип) в первых n
// байтах строки, на которую указывает аргумент str.
void* s21_memchr(const void* str, int c, s21_size_t n) {
  const unsigned char* char_ptr;
  char_ptr = (const unsigned char*)str;
  int flag = 0;

  while (n-- && !flag) {
    if (*char_ptr == (unsigned char)c) {
      flag = 1;
      break;
    }
    char_ptr++;
  }

  return flag ? (void*)char_ptr : s21_NULL;
}

// Сравнивает первые n байтов str1 и str2.
int s21_memcmp(const void* str1, const void* str2, s21_size_t n) {
  const unsigned char* s1 = (const unsigned char*)str1;
  const unsigned char* s2 = (const unsigned char*)str2;
  int dif = 0;

  while (n-- > 0) {
    if (*s1 != *s2) {
      dif = (int)s1[0] - (int)s2[0];
      break;
    }
    s1++;
    s2++;
  }
  return dif;
}

// Копирует n символов из src в dest.
void* s21_memcpy(void* dest, const void* src, s21_size_t n) {
  unsigned char* dest_ptr = (unsigned char*)dest;
  const unsigned char* src_ptr = (const unsigned char*)src;

  while (n--) *dest_ptr++ = *src_ptr++;
  return dest;
}

// Копирует символ c (беззнаковый тип) в первые n символов строки, на которую
// указывает аргумент str
void* s21_memset(void* str, int c, s21_size_t n) {
  unsigned char* p = str;
  while (n--) {
    *p++ = (unsigned char)c;
  }
  return str;
}

// Добавляет строку, на которую указывает src, в конец строки,
// на которую указывает dest, длиной до n символов.
char* s21_strncat(char* dest, const char* src, s21_size_t n) {
  if (n != 0) {
    int i = 0;
    char* d = dest;
    const char* s = src;
    while (*d != '\0') d++;
    do {
      if ((*d = *(s + i)) == '\0') break;
      i++;
      d++;
    } while (--n);
    *d = 0;
  }
  return (dest);
}

// Сравнивает не более первых n байтов str1 и str2.
int s21_strncmp(const char* str1, const char* str2, s21_size_t n) {
  int res = 0;

  if (str1 && str2 && n > 0) {
    for (s21_size_t i = 0; i < n; i++) {
      if (str1[i] == '\0' || str1[i] != str2[i]) {
        res = str1[i] - str2[i];
        break;
      }
    }
  }

  return res;
}

// Копирует до n символов из строки, на которую указывает src, в dest.
char* s21_strncpy(char* dest, const char* src, s21_size_t n) {
  s21_size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }

  for (; i < n; i++) {
    dest[i] = '\0';  // Если длина строки src меньше n, то dest добивает до n
                     // символами '\0'
  }

  return dest;
}

// Вычисляет длину начального сегмента str1, который полностью состоит из
// символов, не входящих в str2.
s21_size_t s21_strcspn(const char* str1, const char* str2) {
  s21_size_t count = 0;

  while (*str1) {
    const char* p = str2;
    int flag = 0;

    while (*p) {
      if (*str1 == *p) {
        flag = 1;
        break;
      }
      p++;
    }

    if (!flag) {
      count++;
    } else {
      break;
    }

    str1++;
  }

  return count;
}

// Выполняет поиск во внутреннем массиве номера ошибки errnum и возвращает
// указатель на строку с сообщением об ошибке. Нужно объявить макросы,
// содержащие массивы сообщений об ошибке для операционных систем mac и linux.
// Описания ошибок есть в оригинальной библиотеке. Проверка текущей ОС
// осуществляется с помощью директив.
char* s21_strerror(int errnum) {
  static char res[BUFF_SIZE] = {'\0'};

  if (errnum < 0 || errnum >= S21_ERRLIST_SIZE) {
    s21_sprintf(res, "Unknown error %d", errnum);
  } else {
    s21_strncpy(res, s21_sys_errlist[errnum], BUFF_SIZE - 1);
  }
  return res;
}

// Вычисляет длину строки str, не включая завершающий нулевой символ.
s21_size_t s21_strlen(const char* str) {
  s21_size_t len = 0;
  while (str[len] != '\0') len++;
  return len;
}

// Находит первый символ в str1, соотв любому символу в str2
const char* s21_strpbrk(const char* str1, const char* str2) {
  char* s1 = (char*)str1;
  char* s2 = (char*)str2;
  const char* res = s21_NULL;
  int len_s2 = s21_strlen(s2);
  int flag = 1;

  while (*s1 && flag) {
    for (int i = 0; i < len_s2 && flag; i++) {
      if (*s1 == s2[i] && flag) {
        res = s1;
        flag = 0;
      }
    }
    s1++;
  }
  return res;
}
// Выполняет поиск первого вхождения символа c (беззнаковый тип) в строке, на
// которую указывает аргумент str
char* s21_strchr(const char* str, int c) {
  char* res = s21_NULL;
  if (str != s21_NULL) {
    while (*str != '\0') {
      if (*str == c) {
        res = (char*)str;
        break;
      }
      str++;
    }
    if (*str == '\0' && c == '\0') {
      res = (char*)str;
    }
  }
  return res;
}

char* s21_strrchr(const char* str,
                  int c) {  // Выполняет поиск последнее вхождение
  const char* res = s21_NULL;

  if (str != s21_NULL) {
    while (*str++) {
      if (*str == (char)c) res = str;
    }
  }
  return (char*)res;
}

// первое вхождение всей строки
const char* s21_strstr(const char* haystack, const char* needle) {
  const char* res = s21_NULL;
  char* s = (char*)haystack;
  int len = s21_strlen(needle);
  int flag = 1;

  while (*s != '\0' && flag) {
    if (s21_strncmp(s, needle, len) == 0) {
      res = s;
      flag = 0;
    } else
      s++;
  }
  return res;
}

// поиск лексем в строке
char* s21_strtok(char* str, const char* delim) {
  static char* mem = NULL;

  if (str) mem = str;

  if (mem) {
    // Найти длину префикса строки, состоящего только из символов в delim
    size_t prefixLen = 0;
    while (mem[prefixLen] && s21_strchr(delim, mem[prefixLen])) ++prefixLen;

    // Переместить указатель str на следующую позицию после префикса
    str = mem + prefixLen;

    // Найти длину сегмента строки, не содержащего символы из delim
    size_t segmentLen = 0;
    while (str[segmentLen] && !s21_strchr(delim, str[segmentLen])) ++segmentLen;

    // Переместить указатель mem на следующий сегмент
    mem = str + segmentLen;

    // Если текущий сегмент не закончен, заменить разделитель нулевым символом
    if (*mem != '\0') {
      *mem = '\0';
      ++mem;
    }
  }

  return *str != '\0' ? str : NULL;
}

void* s21_to_upper(const char* str) {
  char* res;
  if (str) {
    s21_size_t i = 0;
    s21_size_t len = s21_strlen(str) + 1;
    res = (char*)calloc(len, sizeof(char));

    while (str[i] != '\0') {
      res[i] = str[i];
      i++;
    }
    res[i] = '\0';

    for (s21_size_t j = 0; j < len; j++) {
      if (res[j] >= 'a' && res[j] <= 'z') res[j] -= 32;
    }
  } else
    res = s21_NULL;
  return res;
}

void* s21_to_lower(const char* str) {
  char* res;
  if (str) {
    s21_size_t i = 0;
    s21_size_t len = s21_strlen(str) + 1;
    res = (char*)calloc(len, sizeof(char));

    while (str[i] != '\0') {
      res[i] = str[i];
      i++;
    }
    res[i] = '\0';

    for (s21_size_t j = 0; j < len; j++) {
      if (res[j] >= 'A' && res[j] <= 'Z') res[j] += 32;
    }
  } else
    res = s21_NULL;
  return res;
}

char* s21_strcat(char* dest, const char* src) {
  s21_size_t i = 0;
  s21_size_t s = s21_strlen(dest);
  if (dest && src) {
    while (src[i] != '\0') {
      dest[i + s] = src[i];
      i++;
    }
  }
  dest[s + i] = '\0';
  return dest;
}

char* s21_strcpy(char* dest, const char* src) {
  char* r = dest;
  while (*src) {
    *(dest++) = *(src++);
  }
  *dest = '\0';
  return r;
}

// Возвращает новую строку, в которой указанная строка (str) вставлена в
// указанную позицию (start_index) в данной строке (src).
void* s21_insert(const char* src, const char* str, s21_size_t start_index) {
  char* res = s21_NULL;
  if (src && str && start_index <= s21_strlen(src)) {
    s21_size_t src_len = s21_strlen(src);
    s21_size_t str_len = s21_strlen(str);
    s21_size_t res_len = src_len + str_len + 1;
    res = (char*)calloc(res_len, sizeof(char));

    if (res) {
      s21_strncpy(res, src, start_index);
      res[start_index] = '\0';
      s21_strcat(res, str);
      s21_strcat(res, src + start_index);
    }
  }
  return (void*)res;
}

// Возвращает новую строку, в которой удаляются все начальные и конечные
// вхождения набора заданных символов (trim_chars) из данной строки (src).
void* s21_trim(const char* src, const char* trim_chars) {
  char* res = s21_NULL;

  if (src) {
    if (!trim_chars) trim_chars = "";
    char* start = (char*)src;
    char* end = start + s21_strlen(src);
    while (*start && s21_strchr(trim_chars, *start)) {
      start++;
    }

    while (end != start && s21_strchr(trim_chars, *(end - 1))) {
      end--;
    }

    res = (char*)calloc((end - start + 1), sizeof(char));
    if (res) {
      s21_strncpy(res, start, end - start);
      *(res + (end - start)) = '\0';
    }
  }
  return res;
}
