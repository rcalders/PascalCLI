program Snake;

{
  Programme SNAKE pour HP
  Conçu à l'aide de HP PASCAL
  Copyright LDJ 1996
}

uses graphHp, GameHp, CrtHP, SystemHp, Default;

var x, y : integer;
    x2, y2 : integer;
    ax, ay:integer;
    attendre : integer;
    memoire : array [1 .. 80, 1 .. 2] of integer;
    cpt1, cpt2 : integer;
    temps : integer;
    fin : boolean;
      
begin

  DisableInterrupt;

  ClrScr;

  for x:=0 to 130 do putpixel (x, 0);
  for x:=0 to 130 do putpixel (x, 1);
  for y:=0 to 62 do putpixel (0, y);
  for y:=0 to 62 do putpixel (1, y);
  for x:=0 to 130 do putpixel (x, 61);
  for x:=0 to 130 do putpixel (x, 62);
  for y:=0 to 62 do putpixel (129, y);
  for y:=0 to 62 do putpixel (130, y);

  x:=60;
  y:=30;
  ax:=2;
  ay:=0;
  temps := 0;

  cpt1 := 1;
  cpt2 := 5;

  fin := false;

  while not exitpressed and not fin do
  begin

     x:=x+ax-2;
     y:=y+ay-2;

     if LeftPressed then begin ax:=0; ay:=2; end;
     if RightPressed then begin ax:=4; ay:=2; end;
     if UpPressed then begin ax:=2; ay:=0; end;
     if DownPressed then begin ax:=2; ay:=4; end;

     memoire [cpt2, 1] := x;     
     memoire [cpt2, 2] := y;     
     x2 := memoire [cpt1, 1];
     y2 := memoire [cpt1, 2];

     clearpixel (x2, y2);
     clearpixel (x2+1, y2);
     clearpixel (x2, y2+1);
     clearpixel (x2+1, y2+1);

     if point(x, y) then fin := true;

     putpixel (x, y);
     putpixel (x+1, y);
     putpixel (x, y+1);
     putpixel (x+1, y+1);

     temps := temps + 1;
     if temps=50 then temps := 0 else cpt1:=cpt1+1;
     cpt2:=cpt2+1;
     if cpt1=81 then cpt1:=1;
     if cpt2=81 then cpt2 := 1;
     
     for attendre := 1 to 300 do;

  end;

  EnableInterrupt;

end.
