// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: representation.sip,v 1.14.18.3 2007-04-20 14:27:45 amoll Exp $
//

class Representation
  : PropertyManager
{
%TypeHeaderCode
	#include <BALL/VIEW/KERNEL/representation.h>
	using namespace BALL;
	using namespace BALL::VIEW;
%End
  public:
	// BALL_CREATE(Representation);
	enum Properties
	{
		PROPERTY__ALWAYS_FRONT = 0,
		PROPERTY__IS_COORDINATE_SYSTEM
	};
	Representation() throw();
	Representation(ModelType /model_type/, DrawingPrecision /drawing_precision/, DrawingMode /drawing_mode/) throw();
	Representation(const Representation& /representation/) throw();
  ~Representation() throw();
	// const Representation& operator = (const Representation& /representation/) throw();

	void clear() throw();
	
	void setHidden(bool) throw();
	bool isHidden() const throw();

	void setDrawingPrecision(DrawingPrecision /precision/) throw();
	DrawingPrecision getDrawingPrecision() const throw();
	void setSurfaceDrawingPrecision(float /precision/) throw();
	float getSurfaceDrawingPrecision() const throw();

 	DrawingMode getDrawingMode() const throw();
	void setDrawingMode(DrawingMode /mode/) throw();

	Size getTransparency() const throw();
	void setTransparency(Size /value/) throw();

	GeometricObjectList& getGeometricObjects() throw();
	// const GeometricObjectList& getGeometricObjects() const throw();

	void setGeometricObjects(GeometricObjectList&) throw();
	void insert(GeometricObject& /Transfer/) throw();

	PyCompositeList getComposites() const throw();
%MethodCode
	PyCompositeList& my_list = *new PyCompositeList;
	List<const Composite*> org_list = sipCpp->getComposites();
	List<const Composite*>::Iterator it = org_list.begin();
	for (; it != org_list.end(); it++)
	{
		my_list.push_back((Composite*)*it);
	}
	sipRes = &my_list;
%End

	void setComposites(const PyCompositeList&) throw();
%MethodCode
	List<const Composite*> my_list;
	for (PyCompositeList::ConstIterator it = a0->begin(); it != a0->end(); ++it)
	{
		Composite* obj = *it;
		my_list.push_back(obj);
	}
	sipCpp->setComposites(my_list);
%End
	void setComposite(const Composite*) throw();

	ModelProcessor* getModelProcessor() throw();
	// const ModelProcessor* getModelProcessor() const throw();

	void setModelProcessor(ModelProcessor* /Transfer/) throw();

	ColorProcessor* getColorProcessor() throw();
	void setColorProcessor(ColorProcessor* /Transfer/) throw();

	void setModelType(ModelType /type/) throw();
	ModelType getModelType() const throw();

	void setColoringMethod(ColoringMethod /type/) throw();
	ColoringMethod getColoringMethod() const throw();

	void enableModelUpdate(bool);
	void enableColoringUpdate(bool);
	bool modelUpdateEnabled() const;
	bool coloringUpdateEnabled() const;

	String getProperties() const throw();
	bool isValid() const throw();
	void update(bool /rebuild/) throw();
	void clearGeometricObjects() throw();
	const PreciseTime& getModelBuildTime() const throw();
	bool needsUpdate() const throw();
	void setNeedsUpdate() throw();
	//	void dump(std::ostream& s, Size depth) const throw();
	String toString() const throw();
	//bool operator == (const Representation&) const throw();
	//bool operator < (const Representation& object) const throw();
};
