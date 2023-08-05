PROJECT_NAME= s21_string
FLAGS= -Wall -Wextra -Werror -std=c11
CHECKFL = $(shell pkg-config --cflags --libs check)

all: clean $(PROJECT_NAME).a

$(PROJECT_NAME).a: 
	gcc -c $(FLAGS) $(PROJECT_NAME).c $(PROJECT_NAME)_sprintf.c $(PROJECT_NAME)_sscanf.c
	ar rc $(PROJECT_NAME).a $(PROJECT_NAME).o $(PROJECT_NAME)_sprintf.o $(PROJECT_NAME)_sscanf.o
	ranlib $(PROJECT_NAME).a

test: $(PROJECT_NAME).a
	checkmk $(PROJECT_NAME).check > $(PROJECT_NAME)_test.c
	gcc $(FLAGS) $(PROJECT_NAME)_test.c $(PROJECT_NAME).a  -o s21_test $(CHECKFL)
	./s21_test

gcov_report:
	checkmk $(PROJECT_NAME).check > $(PROJECT_NAME)_test.c
	gcc -c $(FLAGS) $(PROJECT_NAME)_test.c -o $(PROJECT_NAME)_test.o
	gcc $(FLAGS) --coverage $(PROJECT_NAME)_test.o $(PROJECT_NAME).c $(PROJECT_NAME)_sprintf.c $(PROJECT_NAME)_sscanf.c  -o s21_test $(CHECKFL)
	./s21_test
	lcov -d ./ --capture --output-file s21_test.info
	genhtml s21_test.info --output-directory report
#	xdg-open ./report/index.html
	open ./report/index.html
	
format: 
	clang-format -style=Google -i *.c *.h

style: 
	clang-format -style=Google -n *.c *.h

clean:
	rm -rf *.o *.a *.out s21_test $(PROJECT_NAME)_test.c *.gcda *.gcno report *.info

lib: clean $(PROJECT_NAME).a
	gcc $(FLAGS) main.c $(PROJECT_NAME).a
	./a.out 
