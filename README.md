# Obvious Objects
An explicit and transparent implementation of fully generic data types based on Axel-Tobias Schreiner's Object Orientated C paper 

All type objects are derived from the fundamental struct Object. We ignore the build-in types for C and we use void pointers to point to all objects. Type and size will be determined by reading the type description pointed to by the first field in every object.

All type descriptions are derived from the fundamental struct Type.

All objects have an initial field which points to a Type Description object for the type.

All "methods" and type-associated fields are defined in the Type Desciption object.

Methods are defined as function pointers which allows them to be dynamically selected by a constructor function.

### TO BE CONTINUED
