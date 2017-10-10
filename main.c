#include "precomp.h"

// keyboard handler
HANDLE hKeyboard;

// event to wait on for keyboard input
HANDLE hEvent;

// input buffer
CHAR line[1024];

NTSTATUS
cliOpenInputDevice (OUT PHANDLE handle, WCHAR* deviceName) // open input device such as keyboards
{
    UNICODE_STRING driver;
    OBJECT_ATTRIBUTES objectAttrs;
    IO_STATUS_BLOCK iosb; // status and information about the requested operation (FILE_CREATED, FILE_OPENED, etc...)
    HANDLE hDriver;
    NTSTATUS status;

    RtlInitUnicodeString (&driver, L"\\Device\\KeyboardClass0"); // make unicode from wchar*, from winternl.h
    
    // initialize the object attributes, from Ntdef.h (ntddk.h)
    InitializeObjectAttributes (&objectAttrs,			// save attrs here
                                &driver, 				// device name here
                                OBJ_CASE_INSENSITIVE, 	// atributes
                                NULL,  					// root dir - do not need
                                NULL); 					// set description, optional

    // open a handle to it, from winternl.h
    status = NtCreateFile (&hDriver, // save handler here 
                           SYNCHRONIZE | GENERIC_READ | FILE_READ_ATTRIBUTES, 	// desired access
                           &objectAttrs,										// objectAttributes
                           &iosb,												// status block
                           NULL,												// initial allocation size, optional
                           FILE_ATTRIBUTE_NORMAL,								// the file attributes, default
                           0,													// type of share access
                           FILE_OPEN,											// specifies what to do
                           FILE_DIRECTORY_FILE,					// the file being created or opened is a directory file
                           NULL,								// pointer to an EA buffer used to pass extended attributes
                           0);									// length of the EA buffer

    // Create an event that will be used to wait on the device
    InitializeObjectAttributes (&objectAttrs, NULL, 0, NULL, NULL);
    status = NtCreateEvent (&hEvent, 			// save attrs here
						    EVENT_ALL_ACCESS,   // all possible access rights to the event object
						    &objectAttrs,   	// objectAttributes
						    1, 					// event type (NotificationEvent)
						    0);					// initial state (TRUE/FALSE)

    // Return the handle
    *handle = hDriver;
    return status;
}

// routine waits for input from an input device
NTSTATUS
cliWaitForInput (IN HANDLE hDriver,       	// device handler
                 IN PVOID buffer,         	// input buffer
                 IN OUT PULONG bufferSize)	// size of input buffer
{
    IO_STATUS_BLOCK iosb; 		// status and information about the requested operation (FILE_CREATED, FILE_OPENED, etc...)
    LARGE_INTEGER byteOffset;
    NTSTATUS status;

    RtlZeroMemory (&iosb, sizeof(iosb)); 			 // fills a block of memory with zeros
    RtlZeroMemory (&byteOffset, sizeof(byteOffset)); // fills a block of memory with zeros

    // Try to read the data
    status = NtReadFile (hDriver, 		// handle to the file object
                         hEvent,  		// a handle to an event object
                         NULL,			// reserved
                         NULL,			// reserved
                         &iosb, 			// status block
                         buffer,			// buffer that receives the data read from the file
                         *bufferSize,    // the size, in bytes, of the buffer pointed to by Buffer
                         &byteOffset,    // starting byte offset in the file where the read operation will begin (LARGE_INTEGER)
                         NULL);			// device and intermediate drivers should set this pointer to NULL (optional)

    // Check if data is pending and indeterminate state
    if (status == STATUS_PENDING) // if I/O is in progress
    {
        // Wait on the data to be read
        status = NtWaitForSingleObject (hEvent, // waits until the hEvent object attains a state of signaled
									   TRUE, 	// the alert can be delivered
								       NULL); 	// timeout 
    }

    // Return status and how much data was read
    *bufferSize = (ULONG)iosb.Information;
    return status;
}

KEYBOARD_INPUT_DATA keyboardData;
ULONG bufferLength = sizeof (KEYBOARD_INPUT_DATA);

void NtProcessStartup (void* StartupArgument) 
{ 
	NTSTATUS status;
	PCHAR command;
		
    // setup keyboard input:
    status = cliOpenInputDevice (&hKeyboard, L"\\Device\\KeyboardClass0");
	
	cliWaitForInput (hKeyboard, &keyboardData, &bufferLength);
}