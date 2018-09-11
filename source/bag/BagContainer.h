#pragma once

#include "BagText.h"
#include "BagInteger.h"
#include "BagFloat.h"
#include "BagBool.h"

// forward declarations of BagArray and BagObject
class BagObject;
class BagArray;
typedef PtrTo<BagObject> PtrToBagObject;
typedef PtrTo<BagArray> PtrToBagArray;

MAKE_PTR_TO_SUB(BagContainer, BagThing) {
    public:
        BagContainer (BagType _type) : BagThing (_type) {}

        virtual f8 sortValue () const { return 0.0; };

        PtrToBagThing get (const Text& path) {
            return const_cast<const BagContainer*>(this)->get (path);
        }

        virtual const PtrToBagThing get (const Text& path) const = 0;

        Text getBagTextX (const Text& path) const {
            PtrToBagThing bagThing = get (path);
            PtrToBagText bagText = ptr_downcast<BagText> (bagThing);
            Text text = bagText->get ();
            return text;
        }

        #define GET_TYPE(bagType, bagTypeType, baseType)                                            \
            baseType get ## bagType (const Text& path) const  {                                     \
                PtrToBagThing bagThing = convert (get (path), BagType::bagTypeType);                \
                if (bagThing) {                                                                     \
                    PtrToBag ## bagType bag ## bagType = ptr_downcast<Bag ## bagType> (bagThing);   \
                    baseType thing = bag ## bagType->get ();                                        \
                    return thing;                                                                   \
                }                                                                                   \
                throw RuntimeError (Text ("BagContainer: can't fetch path (") << path<< ") as (" << BagType::bagTypeType << ")"); \
            }

        GET_TYPE(Text, TEXT, Text);
        GET_TYPE(Integer, INTEGER, s8);
        GET_TYPE(Float, FLOAT, f8);
        GET_TYPE(Bool, BOOL, bool);

        #undef GET_TYPE
        #define GET_TYPE(bagType, baseType)                                                         \
            baseType getBag ## bagType (const Text& path) const  {                                  \
                PtrToBagThing bagThing = get (path);                                                \
                if (bagThing) {                                                                     \
                    PtrToBag ## bagType bag ## bagType = ptr_downcast<Bag ## bagType> (bagThing);   \
                    return bag ## bagType;                                                          \
                }                                                                                   \
                throw RuntimeError (Text ("BagContainer: can't fetch path (") << path << ")");      \
            }

        GET_TYPE(Array, PtrToBagArray);
        GET_TYPE(Object, PtrToBagObject);

        #undef GET_TYPE

};
