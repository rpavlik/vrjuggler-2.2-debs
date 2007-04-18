

Juggler Project Overview


The Juggler Team

 $Date$

----------------------------------------------------------------------------
 Table of Contents



  Introduction

  Compiling

  Modules


        VR Juggler

        VR Juggler Portable Runtime (VPR)

        Juggler Configuration and Control Library (JCCL)

        Gadgeteer

        Sonix

        Tweek

        VRJConfig


  VR Juggler 2.x


Introduction

 Congratulations, you have downloaded the source code to the Juggler
 Project, open source software for virtual reality from the Virtual Reality
 Applications Center at Iowa State University. Within this document, you
 will find information about the new VR Juggler 2.x directory structure
 including descriptions of all the component modules that make up the
 Juggler Project. Instructions for building the individual modules can be
 found within the specific module directories and on the VR Juggler website.


Compiling

 For information on compiling the source code, refer to either the
 INSTALL.txt or INSTALL.html file found in this directory.


Modules

 The Juggler Project is made up of several modules that have evolved from
 the original VR Juggler source tree. Within this section, we describe each
 of the modules, what they are used for, and, when possible, how they relate
 to VR Juggler 1.0.


VR Juggler


Directory

 juggler/modules/vrjuggler


Description

 VR Juggler is still VR Juggler. There are many new features in VR Juggler
 since the 1.0 series, but in essence, it is still the same familiar tool.
 Of the projects listed below, VR Juggler depends on VPR, JCCL, Sonix, and
 Gadgeteer. Its individual components have simply been broken out into
 individually developed modules. More information about the latest version
 of VR Juggler is given in the following section.


VR Juggler Portable Runtime (VPR)


Directory

 juggler/modules/vapor


Description

 VPR is the foundation for all the Juggler modules. It provides an object-
 oriented, cross-platform abstraction layer to common operating system
 features such as threads, semaphores, and sockets. All the other modules
 depend on VPR so that they may be ported easily to other platforms. In VR
 Juggler 1.0, VPR was the collection of directories Threads, SharedMem, and
 Sync. Since then, that code has been collected into a single library and
 greatly extended into what is now VPR.

 VPR itself contains a great deal of platform-specific code. It categorizes
 this code into subsystems, and various subsystems may be chosen depending
 upon the target platform and target threading system. For example, on IRIX,
 there are three possible subsystems: SPROC, POSIX, and Netscape Portable
 Runtime (NSPR). Win32, on the other hand, only uses the NSPR subsystem. The
 subsystems are:


*  SPROC (IRIX only)

*  POSIX (UNIX-based platforms only)

*  NSPR (all platforms, required on Win32 and Mac OS X)


 Because VPR is the foundation for all the other modules, it is important to
 choose the right subsystem.

 Choosing the correct subsystem depends on many factors. For example, which
 of the subsystems are supported on the desired target platform? Are there
 compatibility issues with software outside the Juggler Project? In most
 cases, however, the choice is straightforward. Indeed, IRIX is the only
 platform that presents any real issues. In particular, the use of OpenGL
 Performer from SGI raises important compatibility concerns. SPROC threads
 are the only threads that are stable with OpenGL Performer (on IRIX). POSIX
 threads will work, but applications may sometimes crash on startup. As of
 this writing, OpenGL Performer does not work at all with NSPR. Other
 software may pose similar problems, and it is important to know the needs
 of other tools when configuring VPR.

 On platforms other than IRIX, the choice is relatively simple. If NSPR is
 available, its use is recommended. If NSPR is not available, POSIX threads
 is the next logical choice--at least on UNIX-based platforms. On Win32 and
 Mac OS X, NSPR is required.

 As mentioned above, all the other modules depend on VPR, so it must be
 compiled before anything else. For more information on compiling VPR, refer
 to modules/vapor/INSTALL.txt.


Juggler Configuration and Control Library (JCCL)


Directory

 juggler/modules/jackal


Description

 JCCL provides the configuration layer for VR Juggler and other modules. The
 config chunks and reusable configuration editor JavaBeans come from JCCL.
 These JavaBeans are used by the VR Juggler configuration editor tool,
 VRJConfig (see below). In essence, JCCL is the VR Juggler 1.0 Config
 directory with many new features including XML-based config files.

 To build the JCCL C++ library, CppDOM is required for parsing XML. CppDOM
 is a lightweight XML parser written in C++ and designed to provide an
 interface similar to JDOM. More information can be found at http://xml-
 cppdom.sourceforge.net/.

 To build the configuration editor JavaBeans, JDOM and the Tweek Java API
 are required. Tweek is another module in the Juggler Project, and it will
 be built automatically as a dependency of JCCL. JDOM comes with the Juggler
 Project source code and can be found in the directory juggler/external/
 jdom.


Gadgeteer


Directory

 juggler/modules/gadgeteer


Description

 Gadgeteer is made up of the code that was formerly in the VR Juggler 1.0
 Input directory. It contains all the drivers used for VR Juggler input
 devices, and it contains the high-level Input Manager. The goal with
 Gadgeteer is to mold it into the first dynamically loadable component
 within the VR Juggler microkernel. At this time, it must still be compiled
 in, but we hope to generalize the concepts of VR Juggler Managers to the
 point that the kernel knows nothing about them until they are loaded at run
 time.


Sonix


Directory

 juggler/modules/sonix


Description

 Sonix provides an interface useful to many simple VR and entertainment
 applications to trigger and position sounds in 3D. This library provides
 simple audio sound objects on top of several audio APIs. The interface to
 Sonix is kept very simple in order to get people up and running with sound
 as fast as possible.

 The interface to sonix is kept very simple in order to get people up and
 running with sound as fast as possible. Sonix is reconfigurable allowing
 audio APIs to be safely swapped out at runtime without the dependent
 systems noticing. Systems using this layer expect to be completely
 portable.


Tweek


Directory

 juggler/modules/tweek


Description

 Tweek is a cross-platform, cross-language library consisting of a C++ API
 and a Java API. Its purpose is to provide a convenient mechanism for a
 Java-based GUI to interact with a complex C++ library or application. The
 Tweek GUI can be run in virtual environments, external on a palm top, or on
 the desktop next to a running application. Each of these methods gives a
 familiar 2D widget set that can control applications flexibly. To provide
 this capability, it uses CORBA.

 The Java code in Tweek includes the Tweek Java API and the Tweek JavaBean
 loader application. The Java API is provided to make the use of JavaBeans
 and CORBA simpler for programmers. The JavaBean loader application simply
 loads the Beans it finds and presents them to the user in some sort of
 structured manner (for example, a collection of icons with web-style
 navigation).


VRJConfig

 VRJConfig is the configuration editor for VR Juggler. It is a complete
 rewrite of VjControl, both in terms of the user interface and the back-end
 code. The new interface is based on open discussion and feedback from users
 of VR Juggler 1.0 and VjControl.

 The back-end is much more extensible. It allows users to write plug-in
 editors to simplify complex editing tasks. User-defined configuration
 elements can be handled in a much more dynamic manner as well. No
 modification of VR Juggler or JCCL "system" files must be made
 to get user-defined elements into the editor.

 VRJConfig is itself a JavaBean that is loaded into the Tweek Java GUI.
 VRJConfig uses JavaBeans internally to facilitate user extensions. All in
 all, VRJConfig is a highly dynamic, powerful editor for VR Juggler
 configurations.


VR Juggler 2.x

 TODO... (this is the version number of the next release of Juggler)

