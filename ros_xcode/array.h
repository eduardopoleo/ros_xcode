//
//  array.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-02-03.
//

#ifndef array_h
#define array_h

#include <stdio.h>

#define ADD_ARRAY_ELEMENT(array, element, type) { \
    if (array->size + 1 > array->capacity) { \
        int newCapacity; \
        if (array->capacity < 8) { \
            newCapacity = 8; \
        } else { \
            newCapacity = 2 * array->capacity; \
        } \
        array->list = (type**)realloc(array->list, newCapacity * sizeof(type)); \
        array->capacity = newCapacity; \
    } \
    array->list[array->size] = element; \
    array->size++; \
} \

#define INIT_ARRAY(array, type) { \
    array->size = 0; \
    array->capacity = 0; \
    array->list = NULL; \
} \


#endif /* array_h */
