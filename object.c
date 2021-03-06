#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "object.h"

// access function for getting type of object
const void * type_of(const void * _self) {
    const struct Object * self = _self;
    assert(self && self->type);
    return self->type;
}

// access function for getting size of object
size_t size_of(const void * _self) {
    const struct Type * type = type_of(_self);
    return type->size;
}

// access function for getting the parent type of this type
const void * super(const void * _self) {
    const struct Type * self = _self;
    assert(self && self->super);
    return self->super;
}

// selector helper function for printfn
int printfn(const void * self, FILE * fp) {
    const struct Type * type = type_of(self);
    assert(type->printfn);
    return type->printfn(self, fp);
}

// selector helper function for compare
int compare(const void * self, const void * other) {
    const struct Type * type = type_of(self);
    assert(type->compare);
    return type->compare(self, other);
}

// selector helper function for make
void * ctor(void * _self, va_list * app) {
    const struct Type * type = type_of(_self);
    assert(type->ctor);
    return type->ctor(_self, app);
}

void * super_ctor(const void * _class, void * _self, va_list * app) {
    const struct Type * superclass = super(_class);
    assert(_self && superclass -> ctor);
    return superclass->ctor(_self, app);
}

void * make(const void * _type, ...) {
    const struct Type * type = _type;
    struct Object * object;
    va_list ap;
    assert(type && type->size);
    object = calloc(1, type->size);
    assert(object);
    object->type = type;
    va_start(ap, _type);
    object = ctor(object, & ap);
    va_end(ap);
    return object;
}

// selector helper function for scrap
void * dtor(void * _self) {
    const struct Type * type = type_of(_self);
    assert(type->dtor);
    return type->dtor(_self);
}

void * super_dtor(const void * _class, void * _self) {
    const struct Type * superclass = super(_class);
    assert(_self && superclass->dtor);
    return superclass->dtor(_self);
}

void scrap(void * _self) {
    if(_self)
        free(dtor(_self));
}

static void * Object_ctor(void * _self, va_list * app) {
    struct Object * self = _self;
    return self;
}

static void * Object_dtor(void * _self) {
    struct Object * self = _self;
    return self;
}

static int Object_compare(void * _self, const void * _other) {
    struct Object * self = _self, * other = _other;
    if (self == other) return 0;
    return 1;
}

static int Object_printfn(const void * _self, FILE * fp) { 
    const struct Type * type = type_of(_self);
    return fprintf(fp, "%s at %p\n", type->name, _self);
}

/*  Type is a subclass of Object (it contains an object) so we inherit 
    the fields of Object including function pointers (methods).
    We want to write static methods for this type. 
*/

static void * Type_ctor(void * _self, va_list * app) {
    struct Type * self = _self;
    self->name = va_arg(* app, char *);
    self->super = va_arg(* app, struct Type *);
    self->size = va_arg(* app, size_t);
    assert(self->super); // important that super is not null !
    /* next we do inheritance from the super class by memcopying from the ctor function
    pointer location through the end of the struct */
    const size_t offset = offsetof(struct Type, ctor);
    memcpy( (char *) self + offset, 
            (char *) self->super + offset, 
            size_of(self->super) - offset   );
    /* Now we need to overwrite any methods that have been specified in 
       the Type constructor argument list.  */
    typedef void (* voidf)(); // generic function pointer
    voidf selector;
    va_list ap;
    va_copy(ap,*app);
    
    while ((selector = va_arg(ap, voidf))) {
        voidf method = va_arg(ap, voidf);
        if (selector == (voidf) ctor)
            * (voidf *) & self->ctor = method;
        else if (selector == (voidf) dtor)
            * (voidf *) & self->dtor = method;
        else if (selector == (voidf) compare)
            * (voidf *) & self->compare = method;
        else if (selector == (voidf) printfn)
            * (voidf *) & self->printfn = method;
    }
    return self;
}

static void * Type_dtor(void * _self) {
    struct Type * self = _self;
    fprintf(stderr, "%s: cannot scrap a type definition.\n", self->name);
    return 0;
}
// we simply inherit methods from Object type for compare and printfn


/*  We obtain instances of a Type by calling make() through the
    appropriate Type instance member. Type definitions will be
    statically initialized.
 */

static const struct Type prototype[] = {
    {
            { prototype + 1 },
            "Object",
            prototype,
            sizeof(struct Object),
            Object_ctor,
            Object_dtor,
            Object_compare,
            Object_printfn
    }, {
            { prototype + 1 },
            "Type",
            prototype,
            sizeof(struct Type),
            Type_ctor,
            Type_dtor,
            Object_compare,
            Object_printfn
    }
};

const void * Object = prototype;
const void * Type = prototype+1;

