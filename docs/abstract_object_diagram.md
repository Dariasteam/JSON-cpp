graph BT;
ObjectMap --> ObjectContainer;
ObjectVector --> ObjectContainer;

ObjectFinalBool --> ObjectFinal;
ObjectFinalNumber --> ObjectFinal;
ObjectFinalString --> ObjectFinal;

ObjectContainer --> AbstractObject;
ObjectFinal --> AbstractObject;



