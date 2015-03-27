#ifndef LifeH
#define LifeH

#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\ComCtrls.hpp>

static int SX = 5;
static int SY = 5;
const int MAXX = 400;
const int MAXY = 400;

enum LT_state{DEAD , ALIVE , CLEAR , DYING , BIRTH , STAY};

class LT_stats
{
   public:

   LT_stats() {}

   int live , died , born;    // Current Generation Values
   int Tgen , Tdead , Tborn;  // Total Values

   Reset() { live = 0; died = 0; born = 0; Tgen = 0; Tdead = 0; Tborn = 0; }
};

class TfLife : public TForm
{
__published:
        TShape *Line;
        TButton *bNew;
        TUpDown *nTall_arrow;
        TUpDown *nWide_arrow;
        TEdit *nWide;
        TEdit *nTall;
        TLabel *Ltitle;
        TLabel *Ltitle2;
        TLabel *Label1;
        TButton *bAction;
        TImageList *Faces;
        TTimer *Timer;
        TScrollBar *Speed;
        TLabel *Lscale;
        TScrollBar *Scale;
        TCheckBox *cbGrid;
        TCheckBox *cbDead;
        TCheckBox *cbWrap;
        TLabel *Lspeed;
        TLabel *Lwide;
        TLabel *Ltall;
        TLabel *Lscaleval;
        TLabel *Lspeedval;
        TButton *bMode;
        void __fastcall Setup(TObject *Sender);
        void __fastcall Fix(TObject *Sender);
        void __fastcall Create(TObject *Sender);
        void __fastcall Draw(TObject *Sender);
        void __fastcall MMove(TObject *Sender, TShiftState Shift, int X, int Y);
        void __fastcall MDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall MUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall Go(TObject *Sender);
        void __fastcall Generate(TObject *Sender);
        void __fastcall SpeedChange(TObject *Sender);
        void __fastcall MakeWorld(TObject *Sender);
        void __fastcall Clean(TObject *Sender);
        
        void __fastcall ShowMouse(TObject *Sender, TShiftState Shift, int X, int Y);
        void __fastcall ChangeMode(TObject *Sender);
private:

   int cwidth;
   int cheight;
   int ctall;
   int cwide;
   int cx;
   int cy;
   int lastx;
   int lasty;
   int curx;
   int cury;
   int gridw;
   int gridh;
   LT_stats stats;
   LT_state world[MAXX][MAXY];
   Graphics::TBitmap *buffer;
   bool gameon;

   bool drawdead;
   bool wrap;
   bool mlefton;
   bool mrighton;
   bool grid;
   bool toggle;
   bool addmode;

   int __fastcall DoCalc(int r , int c);
   LT_state __fastcall GetWorld(int x , int y , LT_state = CLEAR);
   void __fastcall DrawFace(int x, int y , LT_state state , TColor c = clNone);

public:

   __fastcall TfLife(TComponent* Owner);

};


extern TfLife *fLife;
#endif
