graph BT;
ObjectMap --> ObjectContainer;
ObjectVector --> ObjectContainer;

ObjectFinalBool --> ObjectFinal;
ObjectFinalNumberInt --> ObjectFinal;
ObjectFinalNumberFloat --> ObjectFinal;
ObjectFinalNumberString --> ObjectFinal;

ObjectContainer --> AbstractObject;
ObjectFinal --> AbstractObject;



