// Project1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "Project1.h"
#include "HPPASCALCompilerCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HP48Access 65536745
#define HP49Access 65536746

// The one and only application object

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // Let's get the command line parameter
            //CString aux=theApp.m_lpCmdLine;

            TCHAR* pCommandLine = ::GetCommandLine();
            int nArgc = 0;
            LPWSTR* pArgv = ::CommandLineToArgvW(pCommandLine, &nArgc);
            int i = 0;
            bool notexit = true;
            bool commandtime = true;
            CString command, parameter;

            CHPPASCALCompilerCtrl* compilateur = new CHPPASCALCompilerCtrl();

            // Default values:
            compilateur->m_memoryRPL = 1024 + 0;    // 1024 + parameter value
            compilateur->m_memorySTACK = 1024;  // parameter value
            compilateur->m_memoryBITMAP = 1024 + 1024; // 1024 + parameter value

            compilateur->m_acces =HP48Access ;    // HP48

            bool parseerror = false;
            while (true)
            {
                if (pArgv[i] == 0) break;
                if (commandtime)
                    command = pArgv[i];
                else
                    parameter = pArgv[i];
                i++;

                if (i == 1) continue;   
                if (command == "-sd" || command == "--source-directory")
                {
                    if (!commandtime)
                    {
                        compilateur->m_directorySource = parameter;
                        commandtime = true;
                    }
                }
                else if (command == "-od" || command == "--output-directory")
                {
                    if (!commandtime)
                    {
                        compilateur->m_directoryDestination = parameter;
                    }
                }
                else if (command == "-ud" || command == "--unit-directory")
                {
                    if (!commandtime)
                    {
                        compilateur->m_directoryUnit = parameter;
                    }
                }
                else if (command == "-rd" || command == "--resource-directory")
                {
                    if (!commandtime)
                    {
                        compilateur->m_directoryResource= parameter;
                    }
                }
                else if (command == "-s" || command == "--source")
                {
                    if (!commandtime)
                    {
                        compilateur->m_fileSource = parameter;
                    }
                }
                else if (command == "-o" || command == "--output")
                {
                    if (!commandtime)
                    {
                        compilateur->m_fileDestination = parameter;
                    }
                }
                else if (command == "-mr" || command == "--memory-rpl")
                {
                    if (!commandtime)
                    {
                        compilateur->m_memoryRPL
                            = StrToInt(parameter) + 1024;   
                    }
                }
                else if (command == "-ms" || command == "--memory-stack")
                {
                    if (!commandtime)
                    {
                        compilateur->m_memorySTACK
                            = StrToInt(parameter);
                    }
                }
                else if (command == "-mb" || command == "--memory-bitmap")
                {
                    if (!commandtime)
                    {
                        compilateur->m_memoryBITMAP
                            = StrToInt(parameter)+1024;
                    }
                }
                else if (command == "-hp49")
                {
                    compilateur->m_acces = HP49Access;
                    commandtime = false;    // Doesn't accept parameter
                }
                else
                {
                    parseerror = true;
                    wprintf((CString) "Unknown command: "+ 
                            (CString) command + (CString) "\r\n");
                        break;
                }
                commandtime=!commandtime;
            }

            if (parseerror)
            {
                nRetCode = 0;
                return nRetCode;
            }

            TCHAR currentDir[1024];
            GetCurrentDirectory(1024, currentDir);
            CString current_dir = (CString)currentDir + (CString)"\\";

         //   wprintf(current_dir + (CString) "\r\n");

            if (compilateur->m_fileSource.IsEmpty())
            {
                wprintf((CString) "Missing pascal source file: -s filesource\r\n");
                nRetCode = 1;
                return nRetCode;
            }

            if (compilateur->m_directorySource.IsEmpty())
            {
                compilateur->m_directorySource = current_dir;
            }
            if (compilateur->m_directoryDestination.IsEmpty())
            {
                compilateur->m_directoryDestination = current_dir;
            }
            if (compilateur->m_directoryResource.IsEmpty())
            {
                compilateur->m_directoryResource = current_dir;
            }

            if (compilateur->m_directoryUnit.IsEmpty())
            {
                wchar_t execDir[1024];

                GetModuleFileName(NULL, execDir, 1024);
                CString executablepath = (CString) execDir;
                CString unitpath = executablepath.Left(
                    executablepath.ReverseFind('\\') );
                    compilateur->m_directoryUnit = unitpath+(CString) "\\units\\";
            }
            if (compilateur->m_fileDestination.IsEmpty())
            {
                CString base = compilateur->m_fileSource.Left(
                    compilateur->m_fileSource.ReverseFind('.'));
                CString ext = (CString) ".h48";
                if (compilateur->m_acces == HP49Access)
                    ext = (CString)".h49";
                compilateur->m_fileDestination = base + ext;
            }

            // wprintf(compilateur->m_directoryUnit + (CString)"\r\n");

            compilateur->CompileAux();

            if (compilateur->m_erreur)
            {
                wprintf((CString)"Error. Message: " + compilateur->m_erreurMessage
                    + (CString) "\r\n");
                if (!compilateur->m_erreurFichier.IsEmpty())
                {
                    wprintf((CString)"File: " +
                        compilateur->m_erreurFichier +
                        (CString)" Line: " +
                        IntToStr(compilateur->m_erreurOrdonnee)
                        + (CString)" Col: " +
                        IntToStr(compilateur->m_erreurAbscisse));
                }
                nRetCode = 1;
            }
            else
            {
                wprintf(L"Build success");
            }
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
