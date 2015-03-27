#ifndef LifeH
#define LifeH

#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\Tabnotbk.hpp>
#include <time.h>

const int SX = 0;       // # of pixels from window to buffer
const int SY = 0;

const int MAXX = 400;   // Max size of world
const int MAXY = 400;

const int BW = 431;     // Drawing buffer size
const int BH = 431;

enum LT_state{DEAD , ALIVE , CLEAR , DYING , BIRTH};

class LT_stats
{
   public:

   int alive , born , died;    // Current Generation Values
   int Tgen , Tborn , Tdied;   // Totals
   int Malive , Mborn, Mdied;

   LT_stats() { Reset(); }
   Reset() { alive = born = died = Tgen = Tborn = Tdied = Malive = Mborn = Mdied = 0; }
};

class TfLife : public TForm
{
 __published:

   TShape *sLine;

   TButton *bGo;
   TButton *bNew;
   TButton *bLoad;
   TButton *bSave;
   TButton *bMode;

   TEdit *nWide;
   TEdit *nTall;

   TUpDown *nTallArrow;
   TUpDown *nWideArrow;

   TLabel *lLife;
   TLabel *lBy;
   TLabel *lName;
   TLabel *lWide;
   TLabel *lTall;
   TLabel *lSpeed;
   TLabel *lSpeedVal;
   TLabel *lScale;
   TLabel *lScaleVal;
   TLabel *lGen;
   TLabel *lGenVal;
   TLabel *lAlive;
   TLabel *lAliveVal;
   TLabel *lTime;
   TLabel *lTimeVal;
   TLabel *lGps;
   TLabel *lGpsVal;
   TLabel *lSBorn;
   TLabel *lSBornVal;
   TLabel *lSDied;
   TLabel *lSDiedVal;
   TLabel *lSTotBorn;
   TLabel *lSTotBornVal;
   TLabel *lSTotDied;
   TLabel *lSTotDiedVal;
   TLabel *lMAlive;
   TLabel *lMAliveVal;
   TLabel *lMBorn;
   TLabel *lMBornVal;
   TLabel *lMDied;
   TLabel *lMDiedVal;

   TTimer *Timer;

   TImageList *Faces;

   TScrollBar *Speed;
   TScrollBar *Scale;

   TCheckBox *cbGrid;
   TCheckBox *cbWrap;
   TCheckBox *cbDead;

   TTabbedNotebook *pStats;


   TMemo *mHow;
   void __fastcall Setup(TObject *Sender);
   void __fastcall Clean(TObject *Sender);
   void __fastcall FixInput(TObject *Sender);
   void __fastcall PressNew(TObject *Sender);
   void __fastcall PressGo(TObject *Sender);
   void __fastcall PressLoad(TObject *Sender);
   void __fastcall PressSave(TObject *Sender);
   void __fastcall ChangeSpeed(TObject *Sender);
   void __fastcall ChangeMode(TObject *Sender);

   void __fastcall mMove(TObject *Sender, TShiftState Shift, int X, int Y);
   void __fastcall mDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall mUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall mShowArrow(TObject *Sender, TShiftState Shift, int X, int Y);

   void __fastcall Draw(TObject *Sender);
   void __fastcall MakeWorld(TObject *Sender);
   void __fastcall Generate(TObject *Sender);


   
 private:

   int cx;
   int cy;

   int gridw;
   int gridh;

   int ctall;
   int cwide;

   int cwidth;
   int cheight;

   int lastx;
   int lasty;

   int curx;
   int cury;

   int holdsecs;

   bool gameon;
   bool addmode;

   bool grid;
   bool wrap;
   bool drawdead;

   bool lefton;
   bool righton;

   bool jumping;
   bool panning;

   DWORD fps;
   time_t start_time;

   LT_stats stats;
   LT_state world[MAXX][MAXY];

   Graphics::TBitmap *buffer;

   int __fastcall Neighbors(int r , int c);
   void __fastcall DrawSquare(int x , int y , LT_state state , TColor c = clNone);

 public:

    __fastcall TfLife(TComponent* Owner);
};

extern TfLife *fLife;

#endif
