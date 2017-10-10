#include "precomp.h"

// keyboard handler
HANDLE hKeyboard;

// event to wait on for keyboard input
HANDLE hEvent;

NTSTATUS
cliOpenInputDevice (OUT PHANDLE handle, WCHAR* deviceName) // open input device such as keyboards
{
    UNICODE_STRING driver;
    OBJECT_ATTRIBUTES objectAttrs;
    IO_STATUS_BLOCK iosb; // status and information about the requested operation (FILE_CREATED, FILE_OPENED, etc...)
    HANDLE hDriver;
    NTSTATUS status;

    RtlInitUnicodeString(&driver, L"\\Device\\KeyboardClass0"); // make unicode from wchar*, from winternl.h
    
    // initialize the object attributes, from Ntdef.h (ntddk.h)
    InitializeObjectAttributes(&objectAttrs,			// save attrs here
                               &driver, 				// device name here
                               OBJ_CASE_INSENSITIVE, 	// atributes
                               NULL,  					// root dir - do not need
                               NULL); 					// set description, optional

    // open a handle to it, from winternl.h
    status = NtCreateFile(&hDriver, // save handler here 
                          SYNCHRONIZE | GENERIC_READ | FILE_READ_ATTRIBUTES, 	// desired access
                          &objectAttrs,									// objectAttributes
                          &iosb,												// status block
                          NULL,													// initial allocation size, optional
                          FILE_ATTRIBUTE_NORMAL,								// the file attributes, default
                          0,													// type of share access
                          FILE_OPEN,											// specifies what to do
                          FILE_DIRECTORY_FILE,					// the file being created or opened is a directory file
                          NULL,									// pointer to an EA buffer used to pass extended attributes
                          0);									// length of the EA buffer

    // Create an event that will be used to wait on the device
    InitializeObjectAttributes(&objectAttrs, NULL, 0, NULL, NULL);
    status = NtCreateEvent(&hEvent, 			// save attrs here
						   EVENT_ALL_ACCESS,    // all possible access rights to the event object
						   &objectAttrs,   // objectAttributes
						   1, 					// event type (NotificationEvent)
						   0);					// initial state (TRUE/FALSE)

    // Return the handle
    *handle = hDriver;
    return status;
}


void NtProcessStartup(void* StartupArgument) 
{ 
	NTSTATUS status;
	PCHAR command;
		
    // setup keyboard input:
    status = cliOpenInputDevice(&hKeyboard, L"\\Device\\KeyboardClass0");
	
    //command = cliGetLine (hKeyboard);
}