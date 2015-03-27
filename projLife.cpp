#include <vcl\vcl.h>
#pragma hdrstop

USEFORM("Life.cpp" , fLife);
USERES("projLife.res");

WINAPI WinMain(HINSTANCE , HINSTANCE , LPSTR , int)
{
   try
   {
      Application->Initialize();
      Application->Title = "Life";
      Application->CreateForm(__classid(TfLife), &fLife);
      Application->Run();
   }
   catch (Exception &exception)
   {
      Application->ShowException(&exception);
   }
   return 0;
}

