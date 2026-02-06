CFLAGS="-I. -Itree_differ -Ihash_table -Ilist_for_operators -D_DEBUG \
-ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar\
-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat\
-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd \
-Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls \
-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict\
-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest\
-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand \
-Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno\
-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old\
-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
-fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame\
-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,\
undefined,unreachable,vla-bound,vptr"

SOURCES="main.cpp lexer/lexer.cpp read_file/read_file.cpp parser/parser.cpp\
         tree/tree.cpp tree/tree_dump.cpp error_handling/verify.cpp"

OUTPUT="program"

INPUT="source"

echo "Compilation..."
g++ $CFLAGS $SOURCES -o $OUTPUT

if [ $? -eq 0 ]; then
    echo "Compilation executed successfully!"
    echo "Running program: ./$OUTPUT"
    echo "====================================================================="
    ./$OUTPUT $INPUT
    echo "====================================================================="
    echo "Program executed."
    read -p "Press Enter to exit..."
else
    echo "Compilation error!"
    echo "Compilation terminated."
    exit 1
fi

