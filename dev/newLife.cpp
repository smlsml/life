#include <vcl\vcl.h>
#pragma hdrstop

#include "Life.h"

#pragma resource "*.dfm"

TfLife *fLife;

__fastcall TfLife::TfLife(TComponent *Owner) : TForm(Owner)
{
}

void __fastcall TfLife::Setup(TObject *Sender)
{
   buffer = new Graphics::TBitmap;

   buffer->Width = 421;
   buffer->Height = 421;

   lastx = -1;
   lasty = -1;

   gridw = 1;
   gridh = 1;

   mlefton = false;
   mrighton = false;

   toggle = false;
   addmode = true;
   
   Create(this);
}

void __fastcall TfLife::Fix(TObject *Sender)
{
   bool lastgrid = grid;

   if(nWide->Text.IsEmpty()) nWide->Text = 0;
   if(nTall->Text.IsEmpty()) nTall->Text = 0;

   if(nWide->Text.ToIntDef(0) < 1 || nWide->Text.ToIntDef(0) > MAXX) nWide->Text = MAXX - 1;
   if(nTall->Text.ToIntDef(0) < 1 || nTall->Text.ToIntDef(0) > MAXY) nTall->Text = MAXY - 1;

   wrap = cbWrap->Checked;
   drawdead = cbDead->Checked;
   grid = cbGrid->Checked;

   if(!grid || Scale->Position == 1)
   {
      gridw = 1;
      gridh = 1;
   }

   if((grid && Scale->Position != 1) || Scale->Position == 13)
   {
      gridw = 0;
      gridh = 0;
   }

   if(lastgrid != grid) MakeWorld(this);
}

void __fastcall TfLife::Create(TObject *Sender)
{
   gameon = true;

   Go(this);
   Fix(this);

   buffer->Canvas->Brush->Color = clGreen;
   buffer->Canvas->FillRect( Rect(0 , 0 , 421 , 421) );

   cwide = nWide->Text.ToInt();
   ctall = nTall->Text.ToInt();

   for(int x = 0; x < MAXX; x++)
   {
      for(int y = 0; y < MAXY; y++)
      {
         world[x][y] = CLEAR;
      }
   }

   MakeWorld(this);

   Draw(this);
}

void __fastcall TfLife::Draw(TObject *Sender)
{
   Canvas->Draw(SX , SY , buffer);
}

void __fastcall TfLife::MMove(TObject *Sender , TShiftState Shift , int X , int Y)
{
   bool draw = false;

   if(lastx >= 0 && lasty >= 0)
   {
      if(GetWorld(lastx , lasty) == ALIVE) DrawFace(lastx , lasty , world[lastx][lasty] , clGreen);
      DrawFace(lastx , lasty , world[lastx][lasty]);

      lastx = -1;
      lasty = -1;

      draw = true;
   }

   if(X > 430)
   {
      Screen->Cursor = crArrow;
      if(draw) Draw(this);
      Lwide->Caption = "Width";
      Ltall->Caption = "Height";
      return;
   }

   curx = X - (cx + SX);
   cury = Y - (cy + SY);

   if(curx >= 0 && cury >= 0)
   {
      curx /= cwidth;
      cury /= cheight;

      if(mlefton)         DrawFace(curx , cury  , GetWorld(0 , 0 , ALIVE));
      else if(mrighton)   DrawFace(curx , cury  , CLEAR);

      if(curx >= 0 && curx < cwide && cury >=0 && cury < ctall)
      {
         Screen->Cursor = crNone;

         Lwide->Caption = curx;
         Ltall->Caption = cury;

         DrawFace(curx , cury , world[curx][cury] , clWhite);
         if(Scale->Position == 13 && (GetWorld(curx , cury) == ALIVE || GetWorld(curx , cury) == DEAD)) DrawFace(curx , cury , world[curx][cury]);

         lastx = curx;
         lasty = cury;

         draw = true;
      }
      else
      {
         Screen->Cursor = crArrow;
         Lwide->Caption = "Width";
         Ltall->Caption = "Height";
      }
   }
   else
   {
      curx = -1;
      cury = -1;
      Screen->Cursor = crArrow;
      Lwide->Caption = "Width";
      Ltall->Caption = "Height";
   }

   if(draw) Draw(this);
}

void __fastcall TfLife::MDown(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y)
{
   if(Button == mbLeft)    mlefton = true;
   if(Button == mbRight)   mrighton = true;

   if(curx >= 0 && cury >= 0)
   {
      if(mlefton)         DrawFace(curx , cury  , GetWorld(0 , 0 , ALIVE));
      else if(mrighton)   DrawFace(curx , cury  , CLEAR);
   }

   Draw(this);
}

void __fastcall TfLife::MUp(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y)
{
   if(Button == mbLeft)    mlefton = false;
   if(Button == mbRight)   mrighton = false;
}

void __fastcall TfLife::DrawFace(int x , int y , LT_state which , TColor c)
{
   int index = 1;

   if(c == clNone)
   {
      switch(which)
      {
         //case DYING:
         case STAY:
         case BIRTH:
         case ALIVE: c = clYellow; index = 1; break;

         //case BIRTH: c = clGreen; index = 2;  break;

         case CLEAR: index = 2; c = clGreen; break;
         
         case DYING:
         case DEAD: index = 2; c = clGreen; if(drawdead) { c = clBlack; index = 0; } break;
      }
   }
   else
   {
      index = 3;
   }

   if(x >= 0 && x < cwide && y >=0 && y < ctall)
   {
      if(Scale->Position == 13 && index < 3)
      {
         Faces->Draw(buffer->Canvas , 1 + cx + x * cwidth , 1 + cy + y * cheight , index);
      }
      else
      {
         buffer->Canvas->Brush->Color = c;
         buffer->Canvas->FillRect( Rect(1 + cx + x * cwidth , 1 + cy + y * cheight , gridw + cx + x * cwidth + cwidth , gridh + cy + y * cheight + cheight) );
      }

      world[x][y] = which;
   }
}

void __fastcall TfLife::Go(TObject *Sender)
{
   if(gameon)
   {
      gameon = false;
      Timer->Enabled = false;
      bAction->Caption = "Start";
   }
   else
   {
      gameon = true;
      Timer->Enabled = true;
      bAction->Caption = "Stop";
   }
}

void __fastcall TfLife::Generate(TObject *Sender)
{
   LT_state current;

   if(!gameon) return;

   for(int a = 0; a < cwide; a++)         //Life Loop
   {
      for(int b = 0; b < ctall; b++)
      {
         current = GetWorld(a , b);

         switch( DoCalc(a , b) )           // switch so that check is only called once
         {
            case 3:
               if(current == DEAD || current == CLEAR)
               {
                  //world[a][b] = GetWorld(0 , 0 , BIRTH);
                  //DrawFace(a , b , ALIVE);
                  DrawFace(a , b , GetWorld(0 , 0 , BIRTH));
                  stats.born++;
               }
               else
               {
                  DrawFace(a , b , STAY);
               }
               stats.live++;
            break;

            case 2:
               if(current == ALIVE || current == STAY)
               {
                  //DrawFace(a , b , GetWorld(0 , 0 , BIR) );
                  DrawFace(a , b , STAY);
                  stats.live++;
               }
               else if(current == DEAD)
               {
                  if(drawdead) DrawFace(a , b , CLEAR);
                  else world[a][b] = CLEAR;
               }               
            break;

            default:
               if(current == ALIVE || current == STAY)
               {
                  //world[a][b] = DYING;
                  DrawFace(a , b , GetWorld(0 , 0 , DYING));
                  stats.died++;
               }
               else if(current == DEAD)
               {
                  if(drawdead) DrawFace(a , b , CLEAR);
                  else world[a][b] = CLEAR;
               }
            break;
         }
      }
   }

   stats.Tgen++;
   stats.Tdead += stats.died;
   stats.Tborn += stats.born;

   toggle = toggle != 1;

   //Lwide->Caption = stats.live;
   //Ltall->Caption = stats.born;
   Draw(this);
}

int __fastcall TfLife::DoCalc(int r , int c)
{
   int x , y , temp = 0;

   for(int a = -1;  a <= 1; a++)     //Across
   {
      for(int b = -1; b <= 1; b++)   //Down
      {
         x = r + a;
         y = c + b;

         if(wrap)
         {
            if(x == -1 || x == cwide)   x = cwide - abs(x);   // These adjust cell checking so that board wraps around...
            if(y == -1 || y == ctall)   y = ctall - abs(y);   // my{30} - y{30} = 0  my{30} - y{1} = 29
         }

         LT_state eh = GetWorld(x , y);
         if(eh == ALIVE || eh == STAY || eh == DYING) temp++;
      }
   }

   if(GetWorld(r , c) == ALIVE || GetWorld(r , c) == STAY)   temp -= 1; // If current index has life, - 1
                                             // You can't be your own neighbor..
   return temp;
}

void __fastcall TfLife::SpeedChange(TObject *Sender)
{
   Timer->Interval = Speed->Position;
   Lspeedval->Caption = Speed->Position;
}

void __fastcall TfLife::MakeWorld(TObject *Sender)
{
   Fix(this);
   
   cwidth = Scale->Position;
   cheight = Scale->Position;
   Lscaleval->Caption = Scale->Position;

   buffer->Canvas->Brush->Color = clGreen;
   buffer->Canvas->FillRect( Rect(0 , 0 , 421 , 421) );

   cx = (421 - cwidth * cwide) / 2;
   cy = (421 - cheight * ctall) / 2;

   buffer->Canvas->Pen->Width = 1;
   buffer->Canvas->Pen->Color = (TColor) 0x00006000;

   if(grid && Scale->Position > 1)
   {
      for(int a = 0; a <= cwide; a++)   // Draws Vertical Lines
      {
         buffer->Canvas->MoveTo(cx + a * cwidth , cy);
         buffer->Canvas->LineTo(cx + a * cwidth , cy + gridh + ctall * cheight);
      }

      for(int a = 0; a <= ctall; a++)   // Draws Horizontal Lines
      {
         buffer->Canvas->MoveTo(cx , cy + a * cheight);
         buffer->Canvas->LineTo(cx + gridw + cwide * cwidth , cy + a * cheight);
      }
   }

   buffer->Canvas->Brush->Color = clBlack;
   buffer->Canvas->FrameRect( Rect(cx , cy , 1 + gridw + cx + cwide * cwidth , 1 + gridh + cy + ctall * cheight) );

   if(!gameon)
   {
      for(int a = 0; a < cwide; a++)
      {
         for(int b = 0; b < ctall; b++)
         {
            DrawFace(a , b , world[a][b]);
         }
      }
   }

   Draw(this);
}

void __fastcall TfLife::Clean(TObject *Sender)
{
   Faces->Free();
   delete buffer;
}

void __fastcall TfLife::ShowMouse(TObject *Sender, TShiftState Shift, int X, int Y)
{
   Screen->Cursor = crArrow;
}

LT_state __fastcall TfLife::GetWorld(int x , int y , LT_state ask)
{
   LT_state temp;

   if(ask == CLEAR) temp = world[x][y];
   else temp = ask;

   if(toggle)
   {
      if(temp == ALIVE)      temp = BIRTH;
      else if(temp == DEAD)  temp = DYING;
      else if(temp == BIRTH) temp = ALIVE;
      else if(temp == DYING) temp = DEAD;
   }

   return temp;
}

void __fastcall TfLife::ChangeMode(TObject *Sender)
{
   if(addmode)  bMode->Caption = "Add Mode";
   else bMode->Caption = "Look Mode";

   addmode = addmode != 1;
}
