.PHONY: all clean
all: stack

COMP=clang++

# CFLAGS = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -Wno-c++11-extensions
CFLAGS = -D _DEBUG

%.o: %.cpp
	$(COMP) -c $< -o $@ $(CFLAGS) 

stack: main.o stack_func.o work_with_log_file.o 
	$(COMP) -o $@ $^ 
# clang++ -o quadratic main.o input.o clear_input.o solver.o output.o comparison.o choose_output_file.o color_printf.o

#ptr_array1: main_ptr_array1.o ptr_func.o swap_string.o  cmd_ln_args.o info_about_file_.o
#	$(COMP) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o 
#main.o: main.cpp *h