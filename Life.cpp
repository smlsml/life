/*
  Life By Steve Lawson © 2001 (Last Revised 03/15/2001 11:34pm)

 [HISTORY]

 -03/07/01 Found my old life code from High School AP Computer Science course
           Started port to windows, was originally on a mac
           Layed ground work, setup types, planned layout and features

 -03/08/01 Started input and drawing functions
           Tested, It works! :)

 -03/09/01 Added window controls and little faces

 -03/10/01 Added options (draw dead, draw grid, wrap around)
           Started "Scale" feature

 -03/11/01 Finished "Scale" feature
           Added mode button, started pan and zoom methods

 -03/12/01 Finished pan and zoom
           Added time & stats; types, window tab control, implementation

 -03/15/01 Added instruction splash


 [THINGS TO DO/TRY]

 - Add load and save functions...
 - Add pre-made "cool" life forms
 - Tweak time display
 - Drop Timer, put generate in while loop -- call ProcessMessages() inside it
 - Try different calculation algo.  Hash? Linked List? OctTrees?

*/

#include <vcl\vcl.h>
#pragma hdrstop

#include "Life.h"

#pragma resource "*.dfm"

TfLife *fLife;

/*****************************************************************************************************************\
| F O R M   F U N C T I O N S                                                                                     |
\*****************************************************************************************************************/

// CONSTRUCTOR ===================================================================================================>
__fastcall TfLife::TfLife(TComponent *Owner) : TForm(Owner)
{
}

// Setup =========================================================================================================>
void __fastcall TfLife::Setup(TObject *Sender)
{
   buffer = new Graphics::TBitmap;

   buffer->Width = BW;
   buffer->Height = BH;

   lastx = -1;
   lasty = -1;

   gridw = 1;
   gridh = 1;

   lefton = false;
   righton = false;

   addmode = true;

   PressNew(this);
}

// CLEAN =========================================================================================================>
void __fastcall TfLife::Clean(TObject *Sender)
{
   Faces->Free();
   if(buffer) delete buffer;
}

// FIX INPUT =====================================================================================================>
void __fastcall TfLife::FixInput(TObject *Sender)
{
   bool lastgrid = grid;

   if(nWide->Text.ToIntDef(0) < 1 || nWide->Text.ToIntDef(0) > MAXX) nWide->Text = MAXX;
   if(nTall->Text.ToIntDef(0) < 1 || nTall->Text.ToIntDef(0) > MAXY) nTall->Text = MAXY;

   wrap = cbWrap->Checked;
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

// PRESS NEW =====================================================================================================>
void __fastcall TfLife::PressNew(TObject *Sender)
{
   holdsecs = 0;
   gameon = false;
   mHow->Visible = true;
   Timer->Enabled = false;
   bGo->Caption = "Start";
   fps = GetTickCount();

   FixInput(this);

   stats.Reset();

   buffer->Canvas->Brush->Color = clBlack;
   buffer->Canvas->FillRect( Rect(0 , 0 , BW , BH) );

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

   lTimeVal->Caption = "0";
   lAliveVal->Caption = "0";
   lGenVal->Caption = "0";
   lGpsVal->Caption = "0";
   lSBornVal->Caption = "0";
   lSDiedVal->Caption = "0";
   lSTotBornVal->Caption = "0";
   lSTotDiedVal->Caption = "0";
   lMAliveVal->Caption = "0";
   lMBornVal->Caption = "0";
   lMDiedVal->Caption = "0";
}

// PRESS GO ======================================================================================================>
void __fastcall TfLife::PressGo(TObject *Sender)
{
   fps = GetTickCount();

   if(gameon)
   {
      gameon = false;
      Timer->Enabled = false;
      bGo->Caption = "Resume";
      holdsecs = difftime(time(NULL) , start_time);
   }
   else
   {
      gameon = true;
      Timer->Enabled = true;
      mHow->Visible = false;
      bGo->Caption = "Pause";
      start_time = time(NULL);
      Generate(this);
   }
}

// PRESS LOAD ====================================================================================================>
void __fastcall TfLife::PressLoad(TObject *Sender)
{
   ShowMessage("Soon! :)");
}

// PRESS SAVE ====================================================================================================>
void __fastcall TfLife::PressSave(TObject *Sender)
{
   ShowMessage("Soon! :)");
}

// CHANGE SPEED ==================================================================================================>
void __fastcall TfLife::ChangeSpeed(TObject *Sender)
{
   if(Speed->Position % 5 != 0) Speed->Position -= Speed->Position % 5;

   Timer->Interval = Speed->Position;
   lSpeedVal->Caption = Speed->Position;
}

// CHANGE MODE ===================================================================================================>
void __fastcall TfLife::ChangeMode(TObject *Sender)
{
   if(addmode)   bMode->Caption = "In Look Mode";
   else          bMode->Caption = "In Add Mode";

   addmode = !addmode;
}

/*****************************************************************************************************************\
| I N P U T   F U N C T I O N S                                                                                   |
\*****************************************************************************************************************/

// MOUSE MOVE ====================================================================================================>
void __fastcall TfLife::mMove(TObject *Sender , TShiftState Shift , int X , int Y)
{
   if(mHow->Visible) return;

   bool draw = false;

   curx = X - (cx + SX);
   cury = Y - (cy + SY);

   if(curx >= 0 && cury >= 0)
   {
      curx /= cwidth;
      cury /= cheight;

      if(addmode)
      {
         if(lastx >= 0 && lasty >= 0)
         {
            if(world[lastx][lasty] == ALIVE || drawdead && world[lastx][lasty] == DEAD)
            {
               DrawSquare(lastx , lasty , world[lastx][lasty] , clGreen);
            }

            DrawSquare(lastx , lasty , world[lastx][lasty]);

            lastx = -1;
            lasty = -1;

            draw = true;
         }

         if(X > 430)
         {
            Screen->Cursor = crArrow;

            lWide->Caption = "Width";
            lTall->Caption = "Height";

            if(draw) Draw(this);
            return;
         }

         if(lefton)         DrawSquare(curx , cury  , ALIVE);
         else if(righton)   DrawSquare(curx , cury  , CLEAR);

         if(curx >= 0 && curx < cwide && cury >=0 && cury < ctall)
         {
            Screen->Cursor = crNone;

            lWide->Caption = curx + 1;
            lTall->Caption = cury + 1;

            DrawSquare(curx , cury , world[curx][cury] , clWhite);

            if(Scale->Position == 13 && (world[curx][cury] == ALIVE || drawdead && world[curx][cury] == DEAD))
            {
               DrawSquare(curx , cury , world[curx][cury]);
            }

            lastx = curx;
            lasty = cury;

            draw = true;
         }
         else
         {
            Screen->Cursor = crArrow;

            lWide->Caption = "Width";
            lTall->Caption = "Height";
         }
      }
      else
      {
         if(X > 430 && !panning)
         {
            lWide->Caption = "Width";
            lTall->Caption = "Height";

            return;
         }

         if(curx >= 0 && curx < cwide && cury >=0 && cury < ctall)
         {
            lWide->Caption = curx + 1;
            lTall->Caption = cury + 1;
         }

         if(lefton)
         {
            if(!panning) { lastx = curx; lasty = cury; }
            curx = (X - (SX + cheight / 2 + cwidth * lastx));
            cury = (Y - (SY + cwidth / 2 + cheight * lasty));
            panning = true;
            MakeWorld(this);
         }
         else if(righton && !jumping)
         {
            if(lasty < cury && lasty < cury - 1)
            {
               jumping = true;
               if(Scale->Position == 1)   MakeWorld(this);
               else Scale->Position--;
            }
            else if(lasty > cury && lasty > cury + 1)
            {
               jumping = true;
               if(Scale->Position == 13)   MakeWorld(this);
               else Scale->Position++;
            }
         }
      }
   }
   else
   {
      curx = -1;
      cury = -1;

      Screen->Cursor = crArrow;

      lWide->Caption = "Width";
      lTall->Caption = "Height";
   }

   if(draw) Draw(this);
}

// MOUSE DOWN ====================================================================================================>
void __fastcall TfLife::mDown(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y)
{
   if(mHow->Visible) return;

   bool draw = false;

   if(Button == mbLeft)    lefton = true;
   if(Button == mbRight)   righton = true;

   if(curx >= 0 && cury >= 0)
   {
      if(addmode)
      {
         draw = true;

         if(lefton)         DrawSquare(curx , cury  , ALIVE);
         else if(righton)   DrawSquare(curx , cury  , CLEAR);
      }
      else
      {
         if(righton)
         {
            lastx = curx;
            lasty = cury;
         }
      }
   }

   if(draw) Draw(this);
}

// MOUSE UP ======================================================================================================>
void __fastcall TfLife::mUp(TObject *Sender , TMouseButton Button , TShiftState Shift , int X , int Y)
{
   if(mHow->Visible)
   {
      mHow->Visible = false;
      return;
   }

   if(Button == mbLeft)
   {
      lefton = false;
      panning = false;
   }

   if(Button == mbRight)
   {
      righton = false;
      jumping = false;      
   }
}

// MOUSE SHOW ARROW ==============================================================================================>
void __fastcall TfLife::mShowArrow(TObject *Sender , TShiftState Shift , int X , int Y)
{
   Screen->Cursor = crArrow;
}

/*****************************************************************************************************************\
| E N G I N E   F U N C T I O N S                                                                                 |
\*****************************************************************************************************************/

// DRAW BUFFER ===================================================================================================>
void __fastcall TfLife::Draw(TObject *Sender)
{
   Canvas->Draw(SX , SY , buffer);
}

// MAKE WORLD ====================================================================================================>
void __fastcall TfLife::MakeWorld(TObject *Sender)
{
   FixInput(this);
   
   cwidth = Scale->Position;
   cheight = Scale->Position;
   lScaleVal->Caption = Scale->Position;

   buffer->Canvas->Brush->Color = clBlack;
   buffer->Canvas->FillRect( Rect(0 , 0 , BW , BH) );

   if(panning)
   {
      cx = curx;
      cy = cury;
   }
   else if(jumping)
   {
      cx = (BW / 2 - (SX + cheight / 2 + cwidth * lastx));
      cy = (BH / 2 - (SY + cwidth / 2 + cheight * lasty));
   }
   else
   {
      cx = (BW - cwidth * cwide) / 2;
      cy = (BH - cheight * ctall) / 2;
   }

   //Prevent active area of buffer from being dragged outside the window...force 2 cell pad
   if(cx > BW - gridw - 2 * cwidth) cx = BW - gridw - 2 * cwidth;
   if(cx < (gridw + (2 * cwidth)) + ((cwide * cwidth) * -1)) cx = (gridw + (2 * cwidth)) + ((cwide * cwidth) * -1);

   if(cy > BH - gridh - 2 * cheight) cy = BW - gridh - 2 * cheight;
   if(cy < (gridh + (2 * cheight)) + ((ctall * cheight) * -1)) cy = (gridh + (2 * cheight)) + ((ctall * cheight) * -1);
   //

   buffer->Canvas->Brush->Color = clGreen;
   buffer->Canvas->FillRect( Rect(cx , cy , cx + gridw + cwidth * cwide , cy + gridh + cheight * ctall) );

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

   buffer->Canvas->Brush->Color = clDkGray;
   buffer->Canvas->FrameRect( Rect(cx , cy , 1 + gridw + cx + cwide * cwidth , 1 + gridh + cy + ctall * cheight) );

   for(int r = 0; r < cwide; r++)
   {
      for(int c = 0; c < ctall; c++)
      {
         if(world[r][c] == ALIVE)   DrawSquare(r , c , world[r][c]);
      }
   }

   Draw(this);
}

// DRAW SQUARE ===================================================================================================>
void __fastcall TfLife::DrawSquare(int x , int y , LT_state which , TColor c)
{
   int index = 1;

   if(c == clNone)
   {
      switch(which)
      {
         case BIRTH:
         case ALIVE: c = clYellow; index = 1; break;

         case CLEAR: index = 2; c = clGreen; break;

         case DYING:
         case DEAD: index = 2; c = clGreen; if(cbDead->Checked) { c = clDkGray; index = 0; } break;
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

// NEIGHBORS =====================================================================================================>
int __fastcall TfLife::Neighbors(int r , int c)
{
   int x , y , num = 0;

   for(int a = -1;  a <= 1; a++)
   {
      for(int b = -1; b <= 1; b++)
      {
         x = r + a;
         y = c + b;

         if(x == -1 || x == cwide)
         {
            if(wrap)   x = cwide - abs(x);
            else continue;
         }

         if(y == -1 || y == ctall)
         {
            if(wrap)   y = ctall - abs(y);
            else continue;
         }
         if(x < 0 || x >= MAXX || y < 0 || y >= MAXY) ShowMessage("AHHH");
         if(world[x][y] == ALIVE || world[x][y] == DYING)   num++;
      }
   }

   if(world[r][c] == ALIVE)   num -= 1;   // You can't be your own neighbor..

   return num;
}

// GENERATE ======================================================================================================>
void __fastcall TfLife::Generate(TObject *Sender)
{
   if(!gameon) return;

   for(int r = 0; r < cwide; r++)
   {
      for(int c = 0; c < ctall; c++)
      {
         switch( Neighbors(r , c) )
         {
            case 3:
               if(world[r][c] == DEAD || world[r][c] == CLEAR)   world[r][c] = BIRTH;
            break;

            case 2: break;

            default:
               if(world[r][c] == ALIVE)   world[r][c] = DYING;
            break;
         }
      }
   }

   for(int r = 0; r < cwide; r++)
   {
      for(int c = 0;  c < ctall; c++)
      {
         switch( world[r][c] )
         {
            case DYING:
               DrawSquare(r , c, DEAD);
               stats.died++;
            break;

            case DEAD:
               if(drawdead)   DrawSquare(r , c, CLEAR);
               else           world[r][c] = CLEAR;
            break;

            case ALIVE:
               stats.alive++;
            break;

            case BIRTH:
               DrawSquare(r , c , ALIVE);
               stats.born++;
               stats.alive++;
            break;
         }
      }
   }

   drawdead = cbDead->Checked;   // set (drawdead) after gen to allow last deads to be cleared

   stats.Tgen++;
   stats.Tdied += stats.died;
   stats.Tborn += stats.born;

   if(stats.alive > stats.Malive)   stats.Malive = stats.alive;
   if(stats.born  > stats.Mborn)    stats.Mborn  = stats.born;
   if(stats.died  > stats.Mdied)    stats.Mdied  = stats.died;

   fps = GetTickCount() - fps;
   if(fps < 1) fps = 1;
   fps = 1000 / fps;

   lTimeVal->Caption = AnsiString( difftime(time(NULL) , start_time) + holdsecs);
   lAliveVal->Caption = stats.alive;
   lGenVal->Caption = stats.Tgen;

   if(fps < 1)   lGpsVal->Caption = "< 1";
   else   lGpsVal->Caption = AnsiString( (int) fps );

   lSBornVal->Caption = stats.born;
   lSDiedVal->Caption = stats.died;
   lSTotBornVal->Caption = stats.Tborn;
   lSTotDiedVal->Caption = stats.Tdied;
   lMAliveVal->Caption = stats.Malive;
   lMBornVal->Caption = stats.Mborn;
   lMDiedVal->Caption = stats.Mdied;

   stats.alive = 0;
   stats.born = 0;
   stats.died = 0;

   Draw(this);

   fps = GetTickCount();
}
