
// This header defines the interface convention used in the valve engine.
// To make an interface and expose it:
//    1. Derive from IBaseInterface.
//    2. The interface must be ALL pure virtuals, and have no data members.
//    3. Define a name for it.
//    4. In its implementation file, use EXPOSE_INTERFACE or EXPOSE_SINGLE_INTERFACE.

// Versioning
// There are two versioning cases that are handled by this:
// 1. You add functions to the end of an interface, so it is binary compatible with the previous interface. In this case,
//    you need two EXPOSE_INTERFACEs: one to expose your class as the old interface and one to expose it as the new interface.
// 2. You update an interface so it's not compatible anymore (but you still want to be able to expose the old interface
//    for legacy code). In this case, you need to make a new version name for your new interface, and make a wrapper interface and
//    expose it for the old interface.

//#if _MSC_VER >= 1300  // VC7
//#include "tier1/interface.h"
//#else
