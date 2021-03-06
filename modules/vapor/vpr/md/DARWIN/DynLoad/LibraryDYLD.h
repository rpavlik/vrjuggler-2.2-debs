/****************** <VPR heading BEGIN do not edit this line> *****************
 *
 * VR Juggler Portable Runtime
 *
 * Original Authors:
 *   Allen Bierbaum, Patrick Hartling, Kevin Meinert, Carolina Cruz-Neira
 *
 ****************** <VPR heading END do not edit this line> ******************/

/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998-2007 by Iowa State University
 *
 * Original Authors:
 *   Allen Bierbaum, Christopher Just,
 *   Patrick Hartling, Kevin Meinert,
 *   Carolina Cruz-Neira, Albert Baker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#ifndef _VPR_LIBRARY_DYLD_H_
#define _VPR_LIBRARY_DYLD_H_

#include <vpr/vprConfig.h>

#include <stdlib.h>
#include <string>

#include <vpr/Util/Assert.h>

#if defined (__GNUC__) && __GNUC__ > 3
#define dl_restrict __restrict
#else
#define dl_restrict
#endif


namespace vpr
{

/** \class LibraryDYLD LibraryDYLD.h vpr/DynLoad/Library.h
 *
 * Low-level class for loading symbols dynamically.  This implementation wraps
 * the Darwin dyld interface, the preferred method for dynamic symbol loading
 * on Mac OS X.  The idea of this class is to provide the basic features needed
 * to load a single shared/dynamic library and to get symbols from it.  No
 * features for path extension are provided here.
 *
 * This class is typedef'd to vpr::Library.
 */
class LibraryDYLD
{
public:
   /**
    * Constructs a named library object.  This is the only mechanism to name
    * the library to be loaded, and once named, the library's name cannot be
    * changed.
    *
    * @param name The name of the library to load.  It must include the actual
    *             name of the library (e.g., "mylib.dll" or "libmylib.so").
    *             The name may be an absolute path or a relative path.
    */
   LibraryDYLD(const std::string& name)
      : mName(name)
      , mLibrary(NULL)
   {
      ;
   }

   /**
    * Default constructor.  Using this constructor will prevent setting a
    * name for the library to be loaded, but the created object may still be
    * useful.  For example, it can be used to provide access to all symbols
    * currently loaded, or ir may be used as an argument to
    * findSymbolAndLibrary().
    *
    * @see findSymbolAndLibrary, load
    */
   LibraryDYLD()
      : mName("")
      , mLibrary(NULL)
   {
      ;
   }

   /**
    * Copy constructor.
    */
   LibraryDYLD(const LibraryDYLD& lib)
      : mName("")
      , mLibrary(NULL)
   {
      copy(lib);
   }

   /**
    * Unloads the library if one has been loaded.
    */
   ~LibraryDYLD();

   /**
    * Overlaoded assignment operator.
    */
   LibraryDYLD& operator=(const LibraryDYLD& lib)
   {
      copy(lib);
      return *this;
   }

   /**
    * Retrieves the name of the library associated with this object.
    */
   const std::string& getName() const
   {
      return mName;
   }

   /**
    * Loads the library named by this object.
    * This function loads and returns a reference to the specified library.
    * The returned reference becomes the library's identity.  The function
    * suppresses duplicate loading if the library is already known by the
    * runtime.
    *
    * @post The library is loaded if the runtime loader can find it.
    *
    * @throw vpr::IOException is thrown if loading the library fails.
    */
   void load();

   /**
    * This function undoes the effect of the load() method.  After calling this
    * method, future references to the library using its identity will be
    * invalid.
    *
    * @pre A library has previously been loaded with load().
    * @post The library is unloaded.
    *
    * @throw vpr::IOException is thrown if unloading the library fails.
    */
   void unload();

   /**
    * Returns whether this library has been loaded from local storage.
    *
    * @return true is returned if this library has been loaded from local
    *         storage, false otherwise.
    */
   bool isLoaded() const
   {
      return mLibrary != NULL;
   }

   /**
    * Finds and returns an untyped reference to the specified symbol in this
    * library.  If no library was loaded previously, all libraries known to the
    * runtime and the main program are searched in an unspecified order.
    *
    * Use this function to look up functions or data symbols in a shared
    * library.  Getting a pointer to a symbol in a library does indicate that
    * the library is available when the search was made.  The runtime does
    * nothing to ensure the continued validity of the symbol.  If the library
    * is unloaded, for instance, the results of any findSymbol() calls become
    * invalid as well.
    *
    * @pre The library must be loaded before calling.
    *
    * @param symbolName The text representation of the symbol to resolve.
    *
    * @return An untyped pointer, possibly NULL.
    *
    * @throw vpr::LibraryException if the library has not been loaded.
    */
   void* findSymbol(const std::string& symbolName);

   /**
    * Finds a symbol in one of the currently loaded libraries, and returns
    * both the symbol and the library in which it was found.
    *
    * @throw vpr::IOException is thrown if an I/O error occurs.
    */
   static void* findSymbolAndLibrary(const std::string& symbolName,
                                     LibraryDYLD& lib);

protected:
   /**
    * Makes a copy of the given vpr::LibraryDYLD into this object.
    */
   void copy(const LibraryDYLD& lib)
   {
      this->mName    = lib.mName;
      this->mLibrary = lib.mLibrary;
   }

   void* internalDlopen(const char* filename, int flag);

   const char* internalDlerror();

   void* internalDlsym(void* dl_restrict handle,
                       const char* dl_restrict symbol);

   int internalDlclose(void* handle);

private:
   std::string mName;      /**< The name of the library */
   void*       mLibrary;   /**< The loaded library handle */
};

} // End of vpr namespace


#endif /* _VPR_LIBRARY_DYLD_H_ */
