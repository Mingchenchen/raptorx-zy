// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: colorRGBAVector.sip,v 1.2.20.1 2007-03-25 21:37:05 oliver Exp $
//

%MappedType PyColorRGBAVector
{
%TypeHeaderCode
	#include <BALL/PYTHON/pyColorRGBAVector.h>
  #include "sipBALLColorRGBA.h"
%End

%ConvertFromTypeCode
	PyObject* pl = PyList_New(0);

	if (pl == NULL) return NULL;

	// Convert the list.

	PyColorRGBAVector::ConstIterator it = sipCpp->begin();
	for (; it != sipCpp->end(); ++it)
	{
		ColorRGBA& obj = *(ColorRGBA*)&*it;
		PyObject *inst = sipConvertFromInstance(&obj, sipClass_ColorRGBA, 0);
		if (inst == NULL || PyList_Append(pl,inst) < 0)
		{
			Py_DECREF(pl);
			return NULL;
		}
	}

	return pl;
%End
 
%ConvertToTypeCode
	// Convert a Python list of ColorRGBA instances to an ColorRGBAVector object on the heap.
	
	if (sipIsErr == NULL) return PyList_Check(sipPy);
 
	PyColorRGBAVector* cl = new PyColorRGBAVector;
 
	for (int i = 0; i < PyList_GET_SIZE(sipPy); ++i)
	{
		ColorRGBA* c = reinterpret_cast<ColorRGBA*>(sipForceConvertTo_ColorRGBA(PyList_GET_ITEM(sipPy,i),sipIsErr));
 
		if (*sipIsErr)
		{
			delete cl;
			return 0;
		}
 
		cl-> push_back(*c);
	}
 
	*sipCppPtr = cl;

	return 1;
%End
};

