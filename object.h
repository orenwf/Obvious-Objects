#ifndef OBJECT_H
#define OBJECT_H

extern const void * Object; /* for make(Object, ...); */
extern const void * Type; /* for make(Type, "name", parent, size, ..., ); */

void * make(const void * type, ...);
void * scrap(void * self);

int compare(const void * self, const void * other);
int printfn(const void * self, FILE * fp);

/* Type is a summary of the properties and behaviors available to an 
   Object or a struct that inherits from an Object or point to an instance of an 
   Object in their first field. */

typedef struct Object Object;
typedef struct Type Type;

struct Object {
    const Type * type;
};

struct Type {
    const Object _; // type descriptor object
    const char * name; // name of the type for printing
    const Type * super; // parent type of this type
    size_t size;
    void * (* ctor) (void * self, va_list * app);
    void * (* dtor) (void * self);
    int (* compare) (const void * self, const void * other);
    int (* printfn) (const void * self, FILE * fp);
};

void * super_ctor(const void * _class, void * _self, va_list * app) {
    const Type * superclass = super(_class);
    assert(_self && superclass -> ctor);
    return superclass->ctor(_self, app);
}

void * super_dtor(const void * _class, void * _self) {
    const Type * superclass = super(_class);
    assert(_self && superclass->dtor);
    return superclass->dtor(_self);
}

#endif
