# Signal
Shitty version of Signal - Slot 

Signal can be used like Qt signals:

Connect(t, Timeout::timeoutWithData, y2, SomeClass::listenMore);

where t is the object for timeoutWithData, y2 is the object for SomeClass::listenMore

Use Signal when there is no argument to emit
Use Signal1<T> when there is argument to emit

Example in test folder :)
