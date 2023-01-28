//
//  object.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-28.
//

#include <stdio.h>
#include "object.h"

Object *initObject(ObjectType type) {
    Object *object = malloc(sizeof(Object));
    object->type = type;
    
    return object;
}

