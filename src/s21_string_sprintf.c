#include "s21_string.h"

int s21_sprintf(char *str, const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *s_temp = str;
  int perc = 0;
  int wch = 0;

  for (const char *f_temp = format; *f_temp != '\0'; ++f_temp) {
    FORMAT form = {0};
    if (perc) {
      perc = 0;

      pars_format(&f_temp, &form, &args);

      switch (form.spec) {
        case '%':
          *s_temp = '%';
          s_temp++;
          break;
        case 'n': {
          int *flag_n = va_arg(args, int *);
          *flag_n = (int)(s_temp - str);
          break;
        }
        case 'd':
        case 'i':
          print_di(&s_temp, &args, form);
          break;
        case 'c':
          wch += print_c(&s_temp, &args, form);
          break;
        case 's':
          wch += print_s(&s_temp, &args, form);
          break;
        case 'f':
          print_f(&s_temp, &args, form);
          break;
        case 'e':
        case 'E':
          print_Ee(&s_temp, &args, form);
          break;
        case 'g':
        case 'G':
          print_Gg(&s_temp, &args, form);
          break;
        case 'u':
          print_u(&s_temp, &args, form);
          break;
        case 'o':
          print_o(&s_temp, &args, form);
          break;
        case 'x':
        case 'X':
          print_Xx(&s_temp, &args, form);
          break;
        case 'p':
          print_p(&s_temp, &args, form);
          break;
      }
    } else if (*f_temp == '%') {
      perc = 1;
    } else {
      *s_temp++ = *f_temp;
    }
  }
  *s_temp = '\0';
  va_end(args);

  return s_temp - str;
}

void pars_format(const char **f, FORMAT *form, va_list *args) {
  form->accuracy = -1;

  while (**f != '\0') {
    if (**f == '-') {
      form->minus = 1;
    } else if (**f == '+') {
      form->sign = 1;
    } else if (**f == ' ') {
      form->space = 1;
    } else if (**f == '#') {
      form->hash = 1;
    } else if (**f == '0') {
      form->zero = 1;
    } else {
      break;
    }
    (*f)++;
  }

  if (is_digit(**f)) {
    while (is_digit(**f)) {
      form->width = form->width * 10 + (**f - '0');
      (*f)++;
    }
  } else if (**f == '*') {
    form->width = va_arg(*args, int);
    (*f)++;
  }

  if (**f == '.') {
    (*f)++;
    form->accuracy = 0;
    if (is_digit(**f)) {
      while (is_digit(**f)) {
        form->accuracy = form->accuracy * 10 + (**f - '0');
        (*f)++;
      }
    } else if (**f == '*') {
      form->accuracy = va_arg(*args, int);
      (*f)++;
    }
  }

  if (is_len(**f)) {
    form->length = **f;
    (*f)++;
  }

  if (is_spec(**f)) form->spec = **f;

  if (**f == '\0') (*f)--;
}

void print_di(char **s, va_list *args, FORMAT form) {
  long int var;
  if (form.length == 'h') {
    var = (short)va_arg(*args, int);
  } else if (form.length == 'l') {
    var = va_arg(*args, long int);
  } else {
    var = va_arg(*args, int);
  }
  if (var == 0 && (form.accuracy || form.space || form.width)) {
    **s = var + '0';
    (*s)++;
  }
  int num;
  if (form.accuracy < 0)
    num = 22;
  else
    num = 22 + form.accuracy;
  char s_di[num];
  s21_memset(s_di, '\0', num);
  int len = 0;

  if (form.accuracy != -1 || var != 0) {
    len = length_str(var, s_di, form.accuracy);
  }

  char sign = '\0';

  if (form.sign || var < 0) {
    len++;
    sign = (var < 0) ? '-' : '+';
  } else if (form.space) {
    len++;
    sign = ' ';
  }
  if (form.zero && sign != '\0') {
    **s = sign;
    (*s)++;
  }

  if (form.width > len && !form.minus) {
    s21_memset(*s, (form.zero) ? '0' : ' ', form.width - len);
    (*s) += form.width - len;
  }

  if (!form.zero && sign != '\0') {
    **s = sign;
    (*s)++;
  }

  s21_strncpy(*s, s_di, num);

  (*s) += len - ((sign == '\0') ? 0 : 1);
  if (form.width > len && form.minus) {
    s21_memset(*s, ' ', form.width - len);
    (*s) += form.width - len;
  }
}

void flags_app(char *str, FORMAT form) {
  if (!form.sign && str[0] != '-' && form.space &&
      (form.spec == 'f' || form.spec == 'e' || form.spec == 'E' ||
       form.spec == 'g' || form.spec == 'G')) {
    char_to_left(str, ' ');
  }

  if (form.sign && str[0] != '-' &&
      (form.spec == 'f' || form.spec == 'e' || form.spec == 'E' ||
       form.spec == 'g' || form.spec == 'G')) {
    char_to_left(str, '+');
  }

  if (form.accuracy != -1 && form.spec != 'f' && form.spec != 'g' &&
      form.spec != 'G') {
    if (form.accuracy == 0 && !s21_strncmp(str, "0", 1)) {
      str[0] = '\0';
    } else {
      int for_hash = 0;

      if (form.hash && form.spec == 'o') {
        for_hash = 1;
      }
      while ((int)s21_strlen(str) < form.accuracy - for_hash) {
        char_to_left(str, '0');
      }
    }
  }

  if ((form.hash && s21_strncmp(str, "0", 2) && str[0] != '\0') ||
      form.spec == 'p') {
    if (form.spec == 'x' || form.spec == 'p') {
      char_to_left(str, 'x');
      char_to_left(str, '0');
      if (!form.sign && form.space) char_to_left(str, ' ');
      if (form.sign) char_to_left(str, '+');
    }
    if (form.spec == 'X') {
      char_to_left(str, 'X');
      char_to_left(str, '0');
      if (!form.sign && form.space) char_to_left(str, ' ');
      if (form.sign) char_to_left(str, '+');
    }
    if (form.spec == 'o') {
      char_to_left(str, '0');
    }
    if ((form.spec == 'f') && !s21_strchr(str, '.')) {
      int len = s21_strlen(str);
      str[len] = '.';
      str[len + 1] = '\0';
    }

    if ((form.spec == 'E' || form.spec == 'e' || form.spec == 'G' ||
         form.spec == 'g') &&
        !s21_strchr(str, '.')) {
      char *position = {0};
      if (form.spec == 'e' || form.spec == 'g') position = s21_strchr(str, 'e');
      if (form.spec == 'E' || form.spec == 'G') position = s21_strchr(str, 'E');

      if (position != s21_NULL) {
        int i = position - str;
        int len = (int)s21_strlen(str);

        for (int j = len; j > i; j--) {
          str[j] = str[j - 1];
        }

        str[i] = '.';
      }
    }
  }
  if (form.minus && form.width != 0) {
    while ((int)s21_strlen(str) < form.width) {
      int len = (int)s21_strlen(str);
      str[len] = ' ';
      str[len + 1] = '\0';
    }

  } else if (form.width != 0 && !form.minus) {
    char ch = ' ';
    if (form.zero) ch = '0';
    while ((int)s21_strlen(str) < form.width) {
      char_to_left(str, ch);
    }
  }

  char search_chars[] = {'-', '+', ' '};
  int i = 0;
  if (str[0] == '0' &&
      (form.spec == 'f' || form.spec == 'E' || form.spec == 'e' ||
       form.spec == 'G' || form.spec == 'g')) {
    while (str[i] != '\0' && str[i] != 'E' && str[i] != 'e') {
      for (int j = 0; j < 3; j++) {
        if (str[i] == search_chars[j]) {
          char temp = str[i];
          str[i] = str[0];
          str[0] = temp;
        }
      }
      i++;
    }
  }
}

void print_u(char **s, va_list *args, FORMAT form) {
  char udigit[50];

  long unsigned int number;

  if (form.length == 'l') {
    number = va_arg(*args, long unsigned int);
  } else if (form.length == 'h') {
    number = (unsigned short int)va_arg(*args, int);
  } else {
    number = (unsigned int)va_arg(*args, int);
  }

  int_to_string(number, udigit, 10, false);
  flags_app(udigit, form);

  int udigit_len = (int)s21_strlen(udigit);
  s21_strncpy(*s, udigit, udigit_len);
  (*s) += udigit_len;
}

void print_o(char **s, va_list *args, FORMAT form) {
  char udigit[50];

  long unsigned int num;
  if (form.length == 'l') {
    num = va_arg(*args, long unsigned int);
  } else if (form.length == 'h') {
    num = (unsigned short int)va_arg(*args, int);
  } else {
    num = (unsigned int)va_arg(*args, int);
  }

  int_to_string(num, udigit, 8, false);
  flags_app(udigit, form);

  int udigit_len = (int)s21_strlen(udigit);
  s21_strncpy(*s, udigit, udigit_len);
  (*s) += udigit_len;
}

int print_c(char **s, va_list *args, FORMAT form) {
  int res = 0;
  if (form.width > 1 && !form.minus) {
    s21_memset(*s, ' ', form.width - 1);
    (*s) += form.width - 1;
  }
  if (form.length == 'l') {
    wchar_t wide_char = (wchar_t)va_arg(*args, int);
    res = put_wchar(s, wide_char);
  } else {
    char c = va_arg(*args, int);
    **s = c;
    (*s)++;
  }

  if (form.width > 1 && form.minus) {
    s21_memset(*s, ' ', form.width - 1);
    (*s) += form.width - 1;
  }
  return res;
}

int print_s(char **s, va_list *args, FORMAT form) {
  s21_size_t len = 0;
  s21_size_t width = (s21_size_t)form.width;
  s21_size_t accuracy = (form.accuracy < 0) ? 0 : (s21_size_t)form.accuracy;
  int wchar = (form.length == 'l') ? 1 : 0;
  int flag = 0, res = 0;

  wchar_t *wline = s21_NULL;
  const char *line = s21_NULL;

  if (wchar) {
    wline = va_arg(*args, wchar_t *);
    if (wline == s21_NULL) {
      wline = L"(null)";
      flag = 1;
    }
    len = wcslen(wline);
  } else {
    line = va_arg(*args, const char *);
    if (line == s21_NULL) {
      line = "(null)";
      flag = 1;
    }
    len = s21_strlen(line);
  }

  if (form.accuracy >= 0 && accuracy < len) {
    len = (flag) ? 0 : accuracy;
  }

  if (width > len && !form.minus) {
    s21_memset(*s, ' ', width - len);
    (*s) += width - len;
  }

  if (wchar) {
    for (s21_size_t i = 0; i < len; i++) {
      res += put_wchar(s, wline[i]);
    }
  } else {
    s21_strncpy(*s, line, len);
    (*s) += len;
  }

  if (width > len && form.minus) {
    s21_memset(*s, ' ', width - len);
    (*s) += width - len;
  }
  return res;
}

void print_p(char **s, va_list *args, FORMAT form) {
  char temp[50];
  void *ptr = va_arg(*args, void *);

  pointer_to_string(ptr, temp);
  flags_app(temp, form);

  int len_temp = (int)s21_strlen(temp);
  s21_strncpy(*s, temp, len_temp);
  (*s) += len_temp;
}

void print_f(char **p, va_list *args, FORMAT form) {
  long double number;
  char buffer[200] = {0};

  if (form.length == 'L') {
    number = va_arg(*args, long double);
  } else {
    number = va_arg(*args, double);
  }
  if (isnan(number)) {
    s21_strncpy(buffer, "nan", 3);
  } else if (isinf(number) > 0) {
    s21_strncpy(buffer, "inf", 3);
  } else if (isinf(number) < 0) {
    s21_strncpy(buffer, "-inf", 4);
  } else {
    if (form.accuracy == -1) form.accuracy = 6;
    double_to_string(number, buffer, form);
  }

  flags_app(buffer, form);
  int buffer_len = (int)s21_strlen(buffer);
  s21_strncpy(*p, buffer, buffer_len);
  (*p) += buffer_len;
}

void print_Xx(char **s, va_list *args, FORMAT form) {
  char buf[100] = {0};

  unsigned long int number;
  // обрабатываем длину
  if (form.length == 'l') {
    number = va_arg(*args, unsigned long int);
  } else if (form.length == 'h') {
    number = (unsigned short int)va_arg(*args, int);
  } else {
    number = (unsigned int)va_arg(*args, int);
  }

  // переводим в 16ый формат и записываем в строку
  if (form.spec == 'x') int_to_string(number, buf, 16, false);
  if (form.spec == 'X') int_to_string(number, buf, 16, true);

  flags_app(buf, form);

  int len = (int)s21_strlen(buf);
  s21_strncpy(*s, buf, len);
  (*s) += len;
}

void print_Ee(char **p, va_list *args, FORMAT form) {
  long double number;
  char buffer[200] = {0};

  if (form.length == 'L') {
    number = va_arg(*args, long double);
  } else {
    number = va_arg(*args, double);
  }
  if (isnan(number)) {
    s21_strncpy(buffer, "nan", 3);
  } else if (isinf(number) > 0) {
    s21_strncpy(buffer, "inf", 3);
  } else if (isinf(number) < 0) {
    s21_strncpy(buffer, "-inf", 4);
  } else {
    double_to_exp(buffer, number, form);
  }
  flags_app(buffer, form);

  int buffer_len = (int)s21_strlen(buffer);
  s21_strncpy(*p, buffer, buffer_len);
  (*p) += buffer_len;
}

void print_Gg(char **p, va_list *args, FORMAT form) {
  char buffer[200] = {0};
  long double number;

  if (form.length == 'L') {
    number = va_arg(*args, long double);
  } else {
    number = va_arg(*args, double);
  }

  if (isnan(number)) {
    s21_strncpy(buffer, "nan", 3);
  } else if (isinf(number) > 0) {
    s21_strncpy(buffer, "inf", 3);
  } else if (isinf(number) < 0) {
    s21_strncpy(buffer, "-inf", 4);
  } else if (number == 0.0) {
    buffer[0] = '0';
    buffer[1] = '\0';
  } else {
    int q = 0;
    if (form.accuracy == -1)
      q = 6;
    else if (form.accuracy == 0)
      q = 1;
    else
      q = form.accuracy;
    long double temp = number;
    int exp_count = 0;
    while (fabsl(temp) < 1.0) {
      temp *= 10;
      exp_count--;
    }
    while (fabsl(roundl((temp * pow(10, form.accuracy)) /
                        pow(10, form.accuracy))) >= 10.0) {
      temp = temp / 10;
      exp_count++;
    }

    if (q > exp_count && exp_count >= -4) {
      form.accuracy = q - (exp_count + 1);
      double_to_string(number, buffer, form);
    } else {
      form.accuracy = q - 1;
      double_to_exp(buffer, number, form);
    }
  }
  flags_app(buffer, form);

  int buffer_len = (int)s21_strlen(buffer);
  s21_strncpy(*p, buffer, buffer_len);
  (*p) += buffer_len;
}

void double_to_exp(char *buffer, long double number, FORMAT form) {
  if (form.accuracy == -1) form.accuracy = 6;

  int exp_count = 0;
  while (fabsl(number) < 1.0) {
    number *= 10;
    exp_count--;
  }

  while (fabsl(roundl((number * pow(10, form.accuracy)) /
                      pow(10, form.accuracy))) >= 10.0) {
    number = number / 10;
    exp_count++;
  }

  double_to_string(number, buffer, form);

  char exp_count_string[20] = {0};
  char sign = '+';
  if (exp_count < 0) sign = '-';
  int_to_string(abs(exp_count), exp_count_string, 10, false);
  if ((int)s21_strlen(exp_count_string) < 2) {
    char_to_left(exp_count_string, '0');
  }
  char_to_left(exp_count_string, sign);

  char exp_char = {0};
  if (form.spec == 'e' || form.spec == 'g') exp_char = 'e';
  if (form.spec == 'E' || form.spec == 'G') exp_char = 'E';

  int i = s21_strlen(buffer);
  buffer[i] = exp_char;
  buffer[i + 1] = '\0';
  s21_strncat(buffer, exp_count_string, sizeof(exp_count_string));
}

void reverse_string(char *str) {
  int len = s21_strlen(str);
  for (int k = 0; k < len / 2; k++) {
    char buf = str[k];
    str[k] = str[len - 1 - k];
    str[len - 1 - k] = buf;
  }
}

void int_to_string(long long unsigned int number, char *str, int base,
                   bool reg) {
  if (number == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }
  int i = 0;
  while (number > 0) {
    int digit = number % base;
    if (base == 16 && digit >= 10) {
      str[i++] = ((reg) ? 'A' : 'a') + (digit - 10);
    } else {
      str[i++] = '0' + digit;
    }
    number /= base;
  }
  str[i] = '\0';
  reverse_string(str);
}

void double_to_string(long double number, char *buffer, FORMAT form) {
  char sign = '\0';

  if (number < 0) {
    sign = '-';
    number = -number;
  }

  long double int_part = 0;
  long double frac_part = modfl(number, &int_part);

  if (frac_part == 0 || form.accuracy == 0) {
    if (round(frac_part) == 1) int_part++;
  }

  int_to_string((long long unsigned int)int_part, buffer, 10, false);

  if ((form.spec != 'g' || form.spec != 'G')) {
    if (form.accuracy != 0) {
      int i = (int)s21_strlen(buffer);
      buffer[i++] = '.';

      while (form.accuracy > 0) {
        double int_frac_part = 0;
        frac_part = modf(frac_part * 10, &int_frac_part);
        buffer[i++] = '0' + (int)int_frac_part;
        form.accuracy--;
      }
      buffer[i] = '\0';

      int plus_one = 0;
      if (frac_part >= 0.5) {
        plus_one = 1;
        i--;
      }

      while (plus_one == 1) {
        if (i == 0) {
          if (buffer[i] == '9') {
            buffer[i] = '0';
          } else {
            buffer[i]++;
            plus_one = 0;
          }
          break;
        } else if (buffer[i] == '9') {
          buffer[i--] = '0';
        } else if (buffer[i] == '.') {
          i--;
        } else {
          buffer[i--]++;
          plus_one = 0;
        }
      }
      if (plus_one) {
        char_to_left(buffer, '1');
      }
    }
  }

  if (sign == '-') {
    char_to_left(buffer, sign);
  }

  if (!form.hash && (form.spec == 'g' || form.spec == 'G') &&
      s21_strchr(buffer, '.')) {
    int i = (int)s21_strlen(buffer);
    while (buffer[i - 1] == '0' || buffer[i - 1] == '.') {
      if (buffer[i - 1] == '.') {
        buffer[i - 1] = '\0';
        break;
      } else {
        buffer[i - 1] = '\0';
        i--;
      }
    }
  }
}

void pointer_to_string(void *ptr, char *buf) {
  uintptr_t var = (uintptr_t)ptr;

  int i = 0;
  while (var != 0) {
    int n = var & 0xF;
    buf[i++] = (n < 10) ? ('0' + n) : ('a' + n - 10);
    var >>= 4;
  }

  if (i == 0) {
    buf[i++] = '0';
  }
  buf[i] = '\0';
  reverse_string(buf);
}

void char_to_left(char *str, char ch) {
  int len = (int)s21_strlen(str);
  for (int i = len + 1; i > 0; i--) {
    str[i] = str[i - 1];
  }
  str[0] = ch;
}

int length_str(long int number, char *str, int min_len) {
  char *s = str;
  int len = 0;
  int spec_case = number == LONG_MIN;
  if (number < 0) {
    number = (spec_case) ? LONG_MAX : -number;
  }

  long int temp = number;
  while (temp) {
    s++;
    temp /= 10;
  }

  int head_zero = min_len - (s - str);
  if (head_zero > 0) {
    s += head_zero;
  } else {
    head_zero = 0;
  }

  *s = '\0';

  do {
    *--s = '0' + (number % 10) + spec_case;
    spec_case = 0;
    number /= 10;
    len++;
  } while (number);

  len += head_zero;

  while (head_zero > 0) {
    *--s = '0';
    head_zero--;
  }

  return len;
}

int put_wchar(char **s, wchar_t wchar) {
  int res = 0;
  int bytes_written = wctomb(*s, wchar);
  if (bytes_written > 0) {
    (*s) += bytes_written;
    res = bytes_written - 1;
  }
  return res;
}
