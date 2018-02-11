#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef void * var;

extern const void * Object; /* for make(Object, ...); */
extern const void * Type; /* for make(Type, "name", parent, size, ..., ); */

void * make(const void * type, ...);
void scrap(void * self);

int compare(const void * self, const void * other);
int printfn(const void * self, FILE * fp);

const void * super(const void * _self);

void * super_ctor(const void * _class, void * _self, va_list * app);
void * super_dtor(const void * _class, void * _self);

/* Type is a summary of the properties and behaviors available to an 
   Object or a struct that inherits from an Object or point to an instance of an 
   Object in their first field. */

struct Object {
    const struct Type * type;
};

struct Type {
    const struct Object _; // type descriptor object
    const char * name; // name of the type for printing
    const struct Type * super; // parent type of this type
    size_t size;
    void * (* ctor) (void * self, va_list * app);
    void * (* dtor) (void * self);
    int (* compare) (const void * self, const void * other);
    int (* printfn) (const void * self, FILE * fp);
};

#endif
