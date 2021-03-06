/*
 * Interface wrapper code.
 *
 *
 * BALL is made available under the Lesser GNU Public License (LGPL).
 * See BALL/source/LICENSE for details.
 */

#include "sipAPIBALL.h"
#include "sipBALLResidueNameColorProcessor.h"

#include "sipBALLColorProcessor.h"
#include "sipBALLResidueNameColorProcessor.h"
#include "sipBALLComposite.h"
#include "sipBALLColorRGBA.h"
#include "sipBALLAtom.h"
#include "sipBALLVector3.h"


sipResidueNameColorProcessor::sipResidueNameColorProcessor() throw(): ResidueNameColorProcessor(), sipPySelf(0)
{
    sipCommonCtor(sipPyMethods,3);
}

sipResidueNameColorProcessor::sipResidueNameColorProcessor(const ResidueNameColorProcessor& a0): ResidueNameColorProcessor(a0), sipPySelf(0)
{
    sipCommonCtor(sipPyMethods,3);
}

sipResidueNameColorProcessor::~sipResidueNameColorProcessor()
{
    sipCommonDtor(sipPySelf);
}

void sipResidueNameColorProcessor::getColor(const Composite& a0,ColorRGBA& a1)
{
    extern void sipVH_BALL_15(sip_gilstate_t,PyObject *,const Composite&,ColorRGBA&);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[0],sipPySelf,NULL,sipNm_BALL_getColor);

    if (!meth)
    {
        ResidueNameColorProcessor::getColor(a0,a1);
        return;
    }

    sipVH_BALL_15(sipGILState,meth,a0,a1);
}

void sipResidueNameColorProcessor::clear() throw()
{
    extern void sipVH_BALL_0(sip_gilstate_t,PyObject *);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[1],sipPySelf,NULL,sipNm_BALL_clear);

    if (!meth)
    {
        ColorProcessor::clear();
        return;
    }

    sipVH_BALL_0(sipGILState,meth);
}

bool sipResidueNameColorProcessor::start() throw()
{
    extern bool sipVH_BALL_4(sip_gilstate_t,PyObject *);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[2],sipPySelf,NULL,sipNm_BALL_start);

    if (!meth)
        return ColorProcessor::start();

    return sipVH_BALL_4(sipGILState,meth);
}


extern "C" {static PyObject *meth_ResidueNameColorProcessor_setTransparency(PyObject *, PyObject *);}
static PyObject *meth_ResidueNameColorProcessor_setTransparency(PyObject *sipSelf,PyObject *sipArgs)
{
    int sipArgsParsed = 0;

    {
        int a0;
        ResidueNameColorProcessor *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"Bi",&sipSelf,sipClass_ResidueNameColorProcessor,&sipCpp,&a0))
        {
            sipCpp->setTransparency(a0);

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_BALL_ResidueNameColorProcessor,sipNm_BALL_setTransparency);

    return NULL;
}


extern "C" {static PyObject *meth_ResidueNameColorProcessor_getColor(PyObject *, PyObject *);}
static PyObject *meth_ResidueNameColorProcessor_getColor(PyObject *sipSelf,PyObject *sipArgs)
{
    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        const Composite * a0;
        ColorRGBA * a1;
        ResidueNameColorProcessor *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJAJA",&sipSelf,sipClass_ResidueNameColorProcessor,&sipCpp,sipClass_Composite,&a0,sipClass_ColorRGBA,&a1))
        {
            try
            {
            (sipSelfWasArg ? sipCpp->ResidueNameColorProcessor::getColor(*a0,*a1) : sipCpp->getColor(*a0,*a1));
            }
            catch (...)
            {
                sipRaiseUnknownException();
                return NULL;
            }

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_BALL_ResidueNameColorProcessor,sipNm_BALL_getColor);

    return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_ResidueNameColorProcessor(void *, sipWrapperType *);}
static void *cast_ResidueNameColorProcessor(void *ptr,sipWrapperType *targetClass)
{
    void *res;

    if (targetClass == sipClass_ResidueNameColorProcessor)
        return ptr;

    if ((res = sipCast_ColorProcessor((ColorProcessor *)(ResidueNameColorProcessor *)ptr,targetClass)) != NULL)
        return res;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_ResidueNameColorProcessor(void *, int);}
static void release_ResidueNameColorProcessor(void *ptr,int state)
{
    if (state & SIP_DERIVED_CLASS)
        delete reinterpret_cast<sipResidueNameColorProcessor *>(ptr);
    else
        delete reinterpret_cast<ResidueNameColorProcessor *>(ptr);
}


extern "C" {static void dealloc_ResidueNameColorProcessor(sipWrapper *);}
static void dealloc_ResidueNameColorProcessor(sipWrapper *sipSelf)
{
    if (sipIsDerived(sipSelf))
        reinterpret_cast<sipResidueNameColorProcessor *>(sipSelf->u.cppPtr)->sipPySelf = NULL;

    if (sipIsPyOwned(sipSelf))
    {
        release_ResidueNameColorProcessor(sipSelf->u.cppPtr,sipSelf->flags);
    }
}


extern "C" {static void *init_ResidueNameColorProcessor(sipWrapper *, PyObject *, sipWrapper **, int *);}
static void *init_ResidueNameColorProcessor(sipWrapper *sipSelf,PyObject *sipArgs,sipWrapper **,int *sipArgsParsed)
{
    sipResidueNameColorProcessor *sipCpp = 0;

    if (!sipCpp)
    {
        if (sipParseArgs(sipArgsParsed,sipArgs,""))
        {
            sipCpp = new sipResidueNameColorProcessor();
        }
    }

    if (!sipCpp)
    {
        const ResidueNameColorProcessor * a0;

        if (sipParseArgs(sipArgsParsed,sipArgs,"JA",sipClass_ResidueNameColorProcessor,&a0))
        {
            try
            {
            sipCpp = new sipResidueNameColorProcessor(*a0);
            }
            catch (...)
            {
                sipRaiseUnknownException();
                return NULL;
            }
        }
    }

    if (sipCpp)
        sipCpp->sipPySelf = sipSelf;

    return sipCpp;
}


extern "C" {static void *forceConvertTo_ResidueNameColorProcessor(PyObject *, int *);}
static void *forceConvertTo_ResidueNameColorProcessor(PyObject *valobj,int *iserrp)
{
    if (*iserrp || valobj == NULL)
        return NULL;

    if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_ResidueNameColorProcessor))
        return sipConvertToCpp(valobj,sipClass_ResidueNameColorProcessor,iserrp);

    sipBadClass(sipNm_BALL_ResidueNameColorProcessor);

    *iserrp = 1;

    return NULL;
}


/* Define this type's super-types. */
static sipEncodedClassDef supers_ResidueNameColorProcessor[] = {{15, 255, 1}};


static PyMethodDef methods_ResidueNameColorProcessor[] = {
    {sipNm_BALL_getColor, meth_ResidueNameColorProcessor_getColor, METH_VARARGS, NULL},
    {sipNm_BALL_setTransparency, meth_ResidueNameColorProcessor_setTransparency, METH_VARARGS, NULL}
};


sipTypeDef sipType_BALL_ResidueNameColorProcessor = {
    0,
    0,
    "BALL.ResidueNameColorProcessor",
    0,
    {0, 0, 1},
    supers_ResidueNameColorProcessor,
    0,
    2, methods_ResidueNameColorProcessor,
    0, 0,
    0,
    init_ResidueNameColorProcessor,
    0,
    0,
    0,
    0,
    0,
    0,
    dealloc_ResidueNameColorProcessor,
    cast_ResidueNameColorProcessor,
    release_ResidueNameColorProcessor,
    forceConvertTo_ResidueNameColorProcessor,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0
};
