
#ifndef __FAULTSIM_MESSAGES_H
#define __FAULTSIM_MESSAGES_H

// WM_SOCKET_NOTIFY and WM_SOCKET_DEAD are used internally by MFC's
// Windows sockets implementation.  For more information, see sockcore.cpp
#ifndef WM_SOCKET_NOTIFY
  #define WM_SOCKET_NOTIFY    0x0373
#endif
#ifndef WM_SOCKET_DEAD
  #define WM_SOCKET_DEAD      0x0374
#endif

#define WM_CONNECT_ACCEPT	      (WM_USER + 2)		// wParam socket handle
                                                // lParam - Not used

#define WM_ENQUEUE_WORKERTHREAD (WM_USER + 3)   // wParam - Not used
                                                // lParam - thread id

#define WM_DISPLAY_TEXT         (WM_USER + 4)   // wParam - LOword(messageType)
                                                //          HIword(document Id)
                                                // lParam - LPSTR (string) to
                                                //          display *AND* free

#define WM_CONFIGURE_THREAD     (WM_USER + 5)   // wParam - # elements in array
                                                // lParam - size_t * 

#define WM_INVALID_PORT         (WM_USER + 6)   // wParam - doc id
                                                // lParam - port value

#define WM_VALID_PORT           (WM_USER + 7)   // wParam - doc id
                                                // lParam - port value

static LPCSTR glb_lpFaultStrings[] =
      {"Close connection before data",
       "Timeout, do nothing with client data",
       "Send partial data back and close connection",
       "Don't accept large uploads, force segmentation",
       "Refuse connection",
       "Reply with garbage",
       "Corrupt data streams",
       "Close connection after all data received",
       "Respond '200' with no data",
       "Give back bad registration id",
       "Respond '204' with data",
       "Distort next update",
       "Turn off listen",
       "Send first byte alone",
       "Half-close TCP to server",
       "No faults injected",
       "Close connection on POST methods",
       "Generate Responses",
       "Respond 200 with HTML data",
       NULL};

#define FAULT_NODATA             0x0
#define FAULT_TIMEOUT            0x1
#define FAULT_PARTIAL            0x2
#define FAULT_SMALL_UPLOADS      0x3
#define FAULT_REFUSE             0x4
#define FAULT_GARBAGE_REPLY      0x5
#define FAULT_CORRUPT_DATASTREAM 0x6
#define FAULT_CLOSE_AFTER_DATA   0x7
#define FAULT_200_NODATA         0x8
#define FAULT_BAD_REGISTRATIONID 0x9
#define FAULT_204_DATA           0xa
#define FAULT_DISTORT_NEXTUPD    0xb
#define FAULT_NO_LISTEN          0xc
#define FAULT_SOLITARY_BYTE      0xd
#define FAULT_HALF_CLOSE         0xe
#define FAULT_NONE               0xf
#define FAULT_NO_POST            0x10
#define FAULT_GENERATE_RESPONSES 0x11
#define FAULT_HTML_RESPONSE      0x12 // HTML body that isn't datastream!

// In theory we log these "types" with their strings and use it
// to change the color on the output...
typedef enum { FAULT_MESSAGE = 1,   // we have injected a fault!
               INFO_MESSAGE,        // informational
               ERROR_MESSAGE,       // yuk! a problem in program
               STATUS_MESSAGE       // some generic status
             }messageType;

typedef struct
{
  LPCSTR lpString;
  int Identifier;
}stringIdentifiers;

static stringIdentifiers glb_lpWSAEstrings[] = {
 { "WSAEWOULDBLOCK",       WSAEWOULDBLOCK       },
 { "WSAEINPROGRESS",       WSAEINPROGRESS       },
 { "WSAEALREADY",          WSAEALREADY          },
 { "WSAENOTSOCK",          WSAENOTSOCK          },
 { "WSAEDESTADDRREQ",      WSAEDESTADDRREQ      },
 { "WSAEMSGSIZE",          WSAEMSGSIZE          },
 { "WSAEPROTOTYPE",        WSAEPROTOTYPE        },
 { "WSAENOPROTOOPT",       WSAENOPROTOOPT       },
 { "WSAEPROTONOSUPPORT",   WSAEPROTONOSUPPORT   },
 { "WSAESOCKTNOSUPPORT",   WSAESOCKTNOSUPPORT   },
 { "WSAEOPNOTSUPP",        WSAEOPNOTSUPP        },
 { "WSAEPFNOSUPPORT",      WSAEPFNOSUPPORT      },
 { "WSAEAFNOSUPPORT",      WSAEAFNOSUPPORT      },
 { "WSAEADDRINUSE",        WSAEADDRINUSE        },
 { "WSAEADDRNOTAVAIL",     WSAEADDRNOTAVAIL     },
 { "WSAENETDOWN",          WSAENETDOWN          },
 { "WSAENETUNREACH",       WSAENETUNREACH       },
 { "WSAENETRESET",         WSAENETRESET         },
 { "WSAECONNABORTED",      WSAECONNABORTED      },
 { "WSAECONNRESET",        WSAECONNRESET        },
 { "WSAENOBUFS",           WSAENOBUFS           },
 { "WSAEISCONN",           WSAEISCONN           },
 { "WSAENOTCONN",          WSAENOTCONN          },
 { "WSAESHUTDOWN",         WSAESHUTDOWN         },
 { "WSAETOOMANYREFS",      WSAETOOMANYREFS      },
 { "WSAETIMEDOUT",         WSAETIMEDOUT         },
 { "WSAECONNREFUSED",      WSAECONNREFUSED      },
 { "WSAELOOP",             WSAELOOP             },
 { "WSAENAMETOOLONG",      WSAENAMETOOLONG      },
 { "WSAEHOSTDOWN",         WSAEHOSTDOWN         },
 { "WSAEHOSTUNREACH",      WSAEHOSTUNREACH      },
 { "WSAENOTEMPTY",         WSAENOTEMPTY         },
 { "WSAEPROCLIM",          WSAEPROCLIM          },
 { "WSAEUSERS",            WSAEUSERS            },
 { "WSAEDQUOT",            WSAEDQUOT            },
 { "WSAESTALE",            WSAESTALE            },
 { "WSAEREMOTE",           WSAEREMOTE           },
 
 {NULL, 0} };

#define SOCKETERROR_STRING(a) (a <= WSAEREMOTE && a >= WSAEWOULDBLOCK ? glb_lpWSAEstrings[a - WSAEWOULDBLOCK].lpString : "unknown")
#endif // end of inclusion test
