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

package org.vrjuggler.tweek.services;

import org.vrjuggler.tweek.beans.BeanRegistry;
import org.vrjuggler.tweek.beans.TweekBean;


/**
 * A proxy to the built-in Tweek Environment Service.  Instantiate this class
 * to get access to the Environment Service.
 */
public class EnvironmentServiceProxy
   implements EnvironmentService
{
   /**
    * Creates a new EnvironmentService proxy to the Tweek Environment Service.
    *
    * @throws RuntimeException if the EnvironmentService Bean could not be
    *                          found.
    */
   public EnvironmentServiceProxy()
      throws RuntimeException
   {
      TweekBean bean = BeanRegistry.instance().getBean("Environment");

      if ( null == bean )
      {
         throw new RuntimeException("Could not find Environment Service");
      }

      if ( bean instanceof EnvironmentService )
      {
         mEnvService = (EnvironmentService) bean;
      }
      else
      {
         throw new RuntimeException("The Bean registered as 'Environment' is not an EnvironmentService implementation.");
      }
   }

   /**
    * Stores the command-line arguments passed to the application when it was
    * started.
    */
   public void setCommandLineArgs(String[] args)
   {
      mEnvService.setCommandLineArgs(args);
   }

   /**
    * Returns the array of command-line arguments.
    */
   public String[] getCommandLineArgs()
   {
      return mEnvService.getCommandLineArgs();
   }

   /**
    * Expands environment variables in the input string.  Environment variables
    * may be specified using either $(variable_name) or ${variable_name}
    * everything between the parentheses or braces will be considered the name
    * of the variables.
    *
    * @param inputStr  the string in which to look for and expand environment
    *                  variables.
    *
    * @return  the input string with the variables replaced by their values.
    */
   public String expandEnvVars(String inputStr)
   {
      return mEnvService.expandEnvVars(inputStr);
   }

   /**
    * Gets the user's HOME environment variable using the JVM instead of the
    * environment variable.
    *
    * @return  the path to the user's home directory
    */
   public String getUserHome()
   {
      return mEnvService.getUserHome();
   }

   /**
    * Returns the absolute path to the platfom-specific directory where
    * preferences files and other application data should be stored.
    */
   public String getAppDataDir()
   {
      return mEnvService.getAppDataDir();
   }

   /**
    * Returns an identifier for the host operationg system.  The value will be
    * one of EnvironmentService.UNIX, EnvironmentService.MacOS, or
    * EnvironmentService.Windows.
    */
   public int getOS()
   {
      return mEnvService.getOS();
   }

   /**
    * Returns the value of the named environment variable or null if the
    * variable is not set in the user's environment.
    *
    * @param envVarName The name of the variable to look up in the user's
    *                   environment.
    *
    * @return A String object containing the value of the named environment
    *         variable if the variable exists in the user's environment.  If
    *         it does not exist, null is returned.
    */
   public String getenv(String envVarName)
   {
      return mEnvService.getenv(envVarName);
   }

   /**
    * Returns the value of the named environment variable or the given
    * default value if the variable is not set in the user's environment.
    *
    * @param envVarName   The name of the variable to look up in the user's
    *                     environment.
    * @param defaultValue The value to return if the named variable is not
    *                     found in the user's environment.
    *
    * @return A String object containing the value of the named environment
    *         variable if the variable exists in the user's environment.  If
    *         it does not exist, the given default value is returned.
    */
   public String getenv(String envVarName, String defaultValue)
   {
      return mEnvService.getenv(envVarName, defaultValue);
   }

   /** The EnvironmentService instance to which this object is proxy. */
   private EnvironmentService mEnvService = null;
}
