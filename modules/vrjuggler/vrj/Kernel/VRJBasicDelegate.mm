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

#include <vrj/vrjConfig.h>

#import <Foundation/NSDictionary.h>
#import <Foundation/NSLock.h>
#import <Foundation/NSNotification.h>
#import <Foundation/NSPathUtilities.h>
#import <AppKit/NSApplication.h>

#include <gadget/Devices/KeyboardMouseDevice/InputAreaCocoa.h>

#import <vrj/Kernel/Kernel.h>
#import <vrj/Kernel/VRJBasicDelegate.h>


NSString* VRJMaxRecentFiles = @"VRJMaxRecentFiles";
NSString* VRJRecentCfgFiles = @"VRJRecentCfgFiles";

@implementation VRJBasicDelegate
   -(id) init
   {
      mLoadConfigs    = YES;
      mMaxRecentFiles = 10;
      mRecentCfgFiles = nil;
      mPrefsFileName  = nil;
      mPrefsDict      = nil;

      NSFileManager* mgr = [NSFileManager defaultManager];
      NSArray* paths =
         NSSearchPathForDirectoriesInDomains(
            NSApplicationSupportDirectory, NSUserDomainMask, YES
         );

      if ( [paths count] > 0 )
      {
         NSString* app_dir = [paths objectAtIndex:0];
         NSString* vrj_app_dir =
            [app_dir stringByAppendingPathComponent:@"VR Juggler"];
#if VPR_OS_RELEASE_MAJOR < 9
         BOOL created = [mgr createDirectoryAtPath:vrj_app_dir
                                        attributes:nil];
#else
         NSError* error;
         BOOL created =
            [mgr       createDirectoryAtPath:vrj_app_dir
                 withIntermediateDirectories:YES
                                  attributes:nil
                                       error:&error];
#endif

         if ( created )
         {
            mPrefsFileName =
               [vrj_app_dir stringByAppendingPathComponent:@"prefs.plist"];
            [mPrefsFileName retain];
         }
         else
         {
#if VPR_OS_RELEASE_MAJOR < 9
            NSLog(@"Failed to create directory %@\n", vrj_app_dir);
#else
            NSLog(@"Failed to create directory %@: %@\n", vrj_app_dir, error);
#endif
         }
      }

      // If we were able to construct the path to the preferences file, then
      // we try to load it into a dictionary.
      if ( mPrefsFileName )
      {
         mPrefsDict =
            [NSDictionary dictionaryWithContentsOfFile:mPrefsFileName];
      }

      // Create a mutable copy of mPrefsDict.
      if ( mPrefsDict )
      {
         mPrefsDict = [mPrefsDict mutableCopy];
      }
      // If we don't have a preferences dictionary at this point, then we
      // will create one.
      else
      {
         mPrefsDict = [[NSMutableDictionary dictionary] retain];
      }

      NSNumber* max_recent = [mPrefsDict objectForKey:VRJMaxRecentFiles];

      if ( max_recent )
      {
         mMaxRecentFiles = [max_recent unsignedCharValue];
      }

      mRecentCfgFiles = [mPrefsDict objectForKey:VRJRecentCfgFiles];

      // Create a mutable copy of mRecentCfgFiles.
      if ( mRecentCfgFiles )
      {
         mRecentCfgFiles = [mRecentCfgFiles mutableCopy];
      }
      else
      {
         mRecentCfgFiles = [[NSMutableArray array] retain];
      }

      NSMenu* file_menu = [self getRecentFilesMenu];

      if ( file_menu )
      {
         const unsigned int count = [mRecentCfgFiles count];

         for ( unsigned int i = 0; i < count; ++i )
         {
            [self insertCfgFileItem:[mRecentCfgFiles objectAtIndex:i]
                              accel:[NSString stringWithFormat:@"%d", i]
                              index:i];
         }
      }

      return [super init];
   }

   -(void) setLoadConfigs:(BOOL) load
   {
      mLoadConfigs = load;
   }

   -(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication*) sencedr
   {
      // We return NO here because we have a different way of shutting down
      // the application. When vrj::Kernel::stop() is invoked, it will cause
      // the application run loop to stop by invoking
      // vrj::CocoaWrapper::stop().
      return NO;
   }

   -(void) applicationWillFinishLaunching:(NSNotification*) aNotification
   {
   }

   -(void) applicationDidFinishLaunching:(NSNotification*) aNotification
   {
      // We're ready to allow windows to open!
      NSConditionLock* lock = gadget::InputAreaCocoa::getWindowLock();
      [lock unlock];
   }

   -(NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication*) sender
   {
      return NSTerminateNow;
   }

   /**
    * Clean up before terminating.
    */
   -(void) applicationWillTerminate:(NSNotification*) aNotification
   {
      if ( mPrefsFileName )
      {
         [mPrefsDict setObject:[NSNumber numberWithUnsignedChar:mMaxRecentFiles]
                        forKey:VRJMaxRecentFiles];
         [mPrefsDict setObject:mRecentCfgFiles
                        forKey:VRJRecentCfgFiles];

         [mPrefsDict writeToFile:mPrefsFileName
                      atomically:YES];
         [mPrefsFileName release];
      }

      vrj::Kernel::instance()->stop();

      // NOTE: This call is to ensure that we wait for the kernel to stop
      // completely before exiting the run loop. This is necessary to prevent
      // race conditions on shutdown caused by the default NSApplication run
      // method implementation calling exit(3) which in turn causes the Juggler
      // singletons to be deleted. We do not want that to happen until after
      // the kernel has stopped itself.
      vrj::Kernel::instance()->doWaitForKernelStop();
   }

   -(BOOL) application:(NSApplication*) theApplication
              openFile:(NSString*) file
   {
      if ( mLoadConfigs )
      {
         [self kernelLoadConfigFile:file];
      }

      return YES;
   }

   -(BOOL) application:(NSApplication*) theApplication
             openFiles:(NSArray*) files
   {
      if ( mLoadConfigs )
      {
         const int count = [files count];
         for ( int i = 0; i < count; ++i )
         {
            [self kernelLoadConfigFile:[files objectAtIndex:i]];
         }
      }

      return YES;
   }

   -(IBAction) loadConfigFile:(id) sender
   {
      [self kernelLoadConfigFile:[sender title]];
   }

   -(void) kernelLoadConfigFile:(NSString*) fileName
   {
      // Load the configuration file.
      vrj::Kernel::instance()->loadConfigFile([fileName UTF8String]);

      // Update the submenu listing the recently opened configuration files
      // to include fileName.
      const NSUInteger index = [mRecentCfgFiles indexOfObject:fileName];

      if ( index != NSNotFound )
      {
         [mRecentCfgFiles removeObjectAtIndex:index];
      }
      else if ( ! [mRecentCfgFiles count] > mMaxRecentFiles )
      {
         [mRecentCfgFiles removeObjectAtIndex:0];
      }

      [mRecentCfgFiles addObject:fileName];

      NSMenu* files_menu = [self getRecentFilesMenu];

      // NOTE: The mMaxRecentFiles + 2 accounts for the separator item and the
      // "Clear Menu" item after the separator.
      if ( files_menu && [files_menu numberOfItems] > mMaxRecentFiles + 2 )
      {
         [files_menu removeItemAtIndex:0];

         for ( int i = 0; i < mMaxRecentFiles; ++i )
         {
            NSMenuItem* item = [files_menu itemAtIndex:i];
            [item setKeyEquivalent:[NSString stringWithFormat:@"%d", i]];
         }
      }

      if ( files_menu )
      {
         const int index = [files_menu indexOfItemWithTitle:fileName];

         if ( index == NSNotFound )
         {
            int insert_index(0);
            const int count = [files_menu numberOfItems];

            for ( int i = 0; i < count; ++i )
            {
               NSMenuItem* item = [files_menu itemAtIndex:i];

               if ( [item isSeparatorItem] )
               {
                  insert_index = i - 1;
                  break;
               }
            }

            [self insertCfgFileItem:fileName
                              accel:[NSString stringWithFormat:@"%d", 0]
                              index:insert_index];
         }
      }
   }

   /**
    * Receives the message sent by the File -> Open menu item. An NSOpenPanel
    * is displayed, and if the user clicks the OK button, the selected .jconf
    * file(s) is (are) loaded.
    *
    * @post If the user clicks the OK button, \c mLastDir has the name of the
    *       directory where the opened panel was when it was closed.
    *
    * @see -kernelLoadConfigFile:
    */
   -(IBAction) openDocument:(id) sender
   {
      NSArray* file_types = [NSArray arrayWithObject:@"jconf"];
      NSOpenPanel* panel = [NSOpenPanel openPanel];

      [panel setAllowsMultipleSelection:YES];

      if ( ! mLastDir )
      {
         mLastDir = NSHomeDirectory();
         [mLastDir retain];
      }

      const int result = [panel runModalForDirectory:mLastDir
                                                file:nil
                                               types:file_types];

      if ( result == NSOKButton )
      {
         [mLastDir release];
         mLastDir = [panel directory];
         [mLastDir retain];

         NSArray* files = [panel filenames];
         const unsigned int count = [files count];

         for ( unsigned int i = 0; i < count; ++i )
         {
            NSString* file = [files objectAtIndex:i];
            [self kernelLoadConfigFile:file];
         }
      }
   }

   /**
    * Receives the message sent by the "Clear Menu" item in the
    * File -> Open Recent submenu.
    *
    * @post \c mRecentCfgFiles is empty.
    */
   -(IBAction) clearRecentDocuments:(id) sender
   {
      NSMenu* menu = [sender menu];
      const int init_size = [menu numberOfItems];

      for ( int i = 0; i < init_size; ++i )
      {
         NSMenuItem* item = [menu itemAtIndex:0];

         if ( [item isSeparatorItem] ||
              [[item title] isEqualToString:@"Clear Menu"] )
         {
            break;
         }
         else
         {
            [menu removeItem:item];
         }
      }

      [mRecentCfgFiles removeAllObjects];
   }

   -(NSMenu*) getRecentFilesMenu
   {
      NSMenu* files_menu = nil;
      NSApplication* app = [NSApplication sharedApplication];
      NSMenuItem* item   = [[app mainMenu] itemWithTitle:@"File"];

      if ( item )
      {
         item = [[item submenu] itemWithTitle:@"Open Recent"];

         if ( item )
         {
            files_menu = [item submenu];
         }
      }

      return files_menu;
   }

   -(NSMenuItem*) insertCfgFileItem:(NSString*) title
                              accel:(NSString*) accel
                              index:(int) index
   {
      NSMenu* menu     = [self getRecentFilesMenu];
      NSMenuItem* item = [menu insertItemWithTitle:title
                                            action:@selector(loadConfigFile:)
                                     keyEquivalent:accel
                                           atIndex:index];
      [item setKeyEquivalentModifierMask:NSCommandKeyMask];
      NSLog(@"Inserted item %@ at index %d in menu %@\n", item, index, menu);

      return item;
   }
@end
