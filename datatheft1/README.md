# Data Theft

to create .exe :

- Open a Command Prompt 
- Navigate to the directory where you saved the .cpp file:
```powershell
cd C:\Users\AnalysisVM\Desktop
```
- Using Microsoft's Compiler (cl.exe)
```powershell
cl /EHsc /Fe:testthief.exe thief.cpp
```
OR

- Using MinGW's Compiler (g++.exe):
```powershell
g++ -o testthief.exe thief.cpp -static
```

The -static flag with MinGW is important as it bundles necessary libraries into the EXE, so it doesn't need external DLLs to run on the Target VM.