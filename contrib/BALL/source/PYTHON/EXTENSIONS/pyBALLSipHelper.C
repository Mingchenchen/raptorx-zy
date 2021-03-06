// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: pyBALLSipHelper.C,v 1.4.28.2 2007-03-28 13:58:08 amoll Exp $

#include <typeinfo>
#include "sipAPIBALL.h"
#include "sipBALLComposite.h"
#include "sipBALLAtom.h"
#include "sipBALLBond.h"
#include "sipBALLPDBAtom.h"
#include "sipBALLAtomContainer.h"
#include "sipBALLFragment.h"
#include "sipBALLResidue.h"
#include "sipBALLNucleotide.h"
#include "sipBALLMolecule.h"
#include "sipBALLProtein.h"
#include "sipBALLNucleicAcid.h"
#include "sipBALLSecondaryStructure.h"
#include "sipBALLChain.h"
#include "sipBALLSystem.h"
#include "sipBALLObject.h"

namespace BALL
{

#define BALL_TO_SIP_MAP_BASECLASS(type)\
	if (dynamic_cast<const type*>(&object) != NULL)\
		return sipConvertFromInstance(&object, sipClass_##type, 0);

	PyObject* pyMapBALLObjectToSip(Composite& object)
	{
		BALL_TO_SIP_MAP_BASECLASS(PDBAtom)
		BALL_TO_SIP_MAP_BASECLASS(Atom)
		BALL_TO_SIP_MAP_BASECLASS(Bond)
		BALL_TO_SIP_MAP_BASECLASS(Residue)
		BALL_TO_SIP_MAP_BASECLASS(SecondaryStructure)
		BALL_TO_SIP_MAP_BASECLASS(Chain)
		BALL_TO_SIP_MAP_BASECLASS(System)
		BALL_TO_SIP_MAP_BASECLASS(Protein)
		BALL_TO_SIP_MAP_BASECLASS(NucleicAcid)
		BALL_TO_SIP_MAP_BASECLASS(Nucleotide)
		BALL_TO_SIP_MAP_BASECLASS(Molecule)
		BALL_TO_SIP_MAP_BASECLASS(Fragment)
		BALL_TO_SIP_MAP_BASECLASS(AtomContainer)

		// last resort - this *should* work!
		return sipConvertFromInstance(&object, sipClass_Composite, 0);
	}
}
