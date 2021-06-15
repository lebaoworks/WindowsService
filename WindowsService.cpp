/*
Sample C Windows Service
2021-6-15
*/


//#define DEBUG
#define SERVICE_NAME L"TEST_SERVICE"
#include <windows.h>
#include <stdio.h>

enum STATUS {
    OK,
    FORCE_SHUTDOWN,
    REGISTER_CONTROLHANDLER_FAIL,
    OPEN_FILE_FAIL,
    WRITE_FILE_FAIL
};

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
int ServiceInit();
void ReportStatus(SERVICE_STATUS_HANDLE hStatus, SERVICE_STATUS* pServiceStatus);
void ControlHandler(DWORD request);


int main()
{
#ifndef DEBUG
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        { (LPWSTR) SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain },
        { NULL, NULL }
    };

    // Start the control dispatcher thread for our service
    StartServiceCtrlDispatcher(ServiceTable);
#else
    ServiceMain(NULL, NULL);
#endif
    return 0;
}

int ServiceInit()
{
#ifndef DEBUG
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler(
        SERVICE_NAME,
        (LPHANDLER_FUNCTION) ControlHandler
    );

    if (hStatus == (SERVICE_STATUS_HANDLE) 0)        
        return REGISTER_CONTROLHANDLER_FAIL;
#endif
    return 0;
}

void ServiceMain(int argc, char** argv)
{
    int status;
    
    //Initalize
    if (status = ServiceInit())
    {
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = status;
        ReportStatus(hStatus, &ServiceStatus);
    }
    
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ReportStatus(hStatus, &ServiceStatus);

    //Func
    MEMORYSTATUS memory;
    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        GlobalMemoryStatus(&memory);
        FILE* f;
        if (fopen_s(&f, "D:/service_log.txt", "a"))
        {
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            ServiceStatus.dwWin32ExitCode = OPEN_FILE_FAIL;
            ReportStatus(hStatus, &ServiceStatus);
            return;
        }
        fprintf_s(f, "OLA\n");
        fclose(f);
        Sleep(2000);
    }
    return;
}

// Control handler function
void ControlHandler(DWORD request)
{
    switch (request)
    {
    case SERVICE_CONTROL_STOP:
        ServiceStatus.dwWin32ExitCode = OK;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        ServiceStatus.dwWin32ExitCode = FORCE_SHUTDOWN;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        break;
    default:
        break;
    }
    ReportStatus(hStatus, &ServiceStatus);
}

void ReportStatus(SERVICE_STATUS_HANDLE hStatus, SERVICE_STATUS* pServiceStatus)
{
#ifndef DEBUG
    SetServiceStatus(hStatus, pServiceStatus);
#endif
}