#include "MObject.h"
unsigned int MObject::CurrentID = 0;


MObject::MObject() : avaliable(true)
{
	instanceID = CurrentID;
	CurrentID++;
}