#include "symbol.h"

table_t *symbol_table_ctor() {
    table_t *symbol_table = (table_t *)calloc(1, sizeof(table_t));
    symbol_table->data = (symbol_t *)calloc(1, sizeof(symbol_t));
    symbol_table->capacity = 1;
    symbol_table->size = 0;

    return symbol_table;
}

void symbol_table_dtor(table_t *symbol_table) {
    if (symbol_table) {
        free(symbol_table->data);
        free(symbol_table);
    }
}

void symb_table_insert(table_t *symbol_table, symbol_t elem) {
    if (symbol_table->size == symbol_table->capacity) {
        symbol_table->capacity *= 2;
        symbol_table->data = (symbol_t*)realloc(symbol_table->data, symbol_table->capacity);
    }

    uint32_t ind = symbol_table->size;
    symbol_table->data[ind] = elem;
}
