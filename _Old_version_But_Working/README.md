# Code of an Old Version

Code was created in VC++ and the project can be directly appilcable in C++11 or hgher. It use some VC++ non-standard libraries.  

The entire library and some projects that use the librires are in [00. BNPDDEngine](00.%20BNPDDEngine/).

The rest of folders contains more exact analysis of some examples.


# Main Engine

The main engine is in [MainEngineLibrary](MainEngineLibrary). it should be compled as a library.
Each project should has this library to be added and contains includes:

`#include "..\MainEngineLibrary\PDDEngineManager.h"`<br>
`#include "..\PDDErrorsAndConst.h"`


