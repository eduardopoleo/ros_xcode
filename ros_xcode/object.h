//
//  object.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-24.
//

#ifndef object_h
#define object_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum ObjectType {
    NUMBER_OBJ,
    STRING_OBJ,
    BOOLEAN_OBJ
} ObjectType;

typedef struct Object {
    ObjectType type;
    union {
        struct {
           double value;
        } number;

        struct {
            int length;
            char *value;
        } string;
        
        struct {
            bool value;
        } boolean;
    } as;
} Object;

Object *initObject(ObjectType type);

#endif /* object_h */
