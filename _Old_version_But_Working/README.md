# Code of an Old Version

Code was created in VC++ and the project can be directly applicable in C++11 or higher. It used some VC++ non-standard libraries.  The current version was fixed and compatible with C++14 x64 version.

The entire library and some projects that use the librires are in [00. BNPDDEngine](00.%20BNPDDEngine/).

The rest of folders contains more exact analysis of some examples.


# Main Engine

The main engine is in [MainEngineLibrary](MainEngineLibrary). it should be compiled as a library.
Each project should has this library to be added and contains includes:

`#include "..\MainEngineLibrary\PDDEngineManager.h"`<br>
`#include "..\PDDErrorsAndConst.h"`


The previous version was not compatible with C++14 and higher. The 2022.05.19 version is compatible with newer standards for Win32 code. The x64 version is still in development. 

