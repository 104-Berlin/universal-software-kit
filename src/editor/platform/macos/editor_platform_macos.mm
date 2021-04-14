#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include "engine.h"
#include "prefix_editor.h"

using namespace Editor;

EVector<EString> Platform::OpenFileDialog(const EString& title, const EVector<EString>& filters, const EString& defaultPath) 
{
   int i;
   EVector<EString> fileList;
   // Create a File Open Dialog class.
   NSOpenPanel* openDlg = [NSOpenPanel openPanel];
   [openDlg setLevel:CGShieldingWindowLevel()];
   // Set array of file types

   NSMutableArray * fileTypesArray = [NSMutableArray array];
   for (i = 0;i < filters.size(); i++)
   {
      NSString * filt =[NSString stringWithUTF8String:filters[i].c_str()];
      [fileTypesArray addObject:filt];
   }

   // Enable options in the dialog.
   [openDlg setCanChooseFiles:YES];
   [openDlg setAllowedFileTypes:fileTypesArray];
   [openDlg setAllowsMultipleSelection:TRUE];
   [openDlg setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String:defaultPath.c_str() ] ] ];

   // Display the dialog box. If the OK pressed,
   // process the files.
   if ( [openDlg runModal] == NSModalResponseOK ) {
      // Gets list of all files selected
      NSArray *files = [openDlg URLs];
      // Loop through the files and process them.
      for( i = 0; i < [files count]; i++ ) {
         // Do something with the filename.
        fileList.push_back(EString([[[files objectAtIndex:i] path] UTF8String]));
         }
      }
   return fileList;
}


EString Platform::SaveFileDialog(const EString& title, const EVector<EString>& allowedEndings) 
{
   NSSavePanel *panel = [NSSavePanel savePanel];

   NSMutableArray * endings = [NSMutableArray array];
   for (size_t i = 0;i < allowedEndings.size(); i++)
   {
      NSString * filt =[NSString stringWithUTF8String:allowedEndings[i].c_str()];
      [endings addObject:filt];
   }

   [panel setMessage:@"Save File."]; // Message inside modal window
   [panel setAllowsOtherFileTypes:YES];
   [panel setCanCreateDirectories:YES];
   [panel setAllowedFileTypes:endings];
   [panel setTitle:[NSString stringWithUTF8String:title.c_str()]]; // Window title


   if ([panel runModal] == NSModalResponseOK) {     
      return EString([[[panel URL] path] UTF8String]);
   }   
   return "";
}