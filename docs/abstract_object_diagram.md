graph BT;
ObjectMap --> ObjectContainer;
ObjectVector --> ObjectContainer;

ObjectFinalBool --> ObjectFinal;
ObjectFinalNumber --> ObjectFinal;
ObjectFinalNumber --> ObjectFinal;
ObjectFinalNumberString --> ObjectFinal;

ObjectContainer --> AbstractObject;
ObjectFinal --> AbstractObject;



