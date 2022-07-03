PROGRAM Tetris;

{
  Programme Tetris par de Jouvencel Ludovic
  Conçu à l'aide de HP PASCAL
  Version 1.0 le 11/08/1996
}

{ Les librairies de fonctions à importer }

USES CrtHp, GameHp, MathsHp, SystemHp;

{ Librairie graphique }

RESSOURCE tetris;

{ Les constantes utilisées par le programme }

STATIC Pieces : array [1..7] of string[16] =
         '0000010011100000', '0010001000100010', '0000011100010000', 
         '0000011001100000', '0000111010000000', '0000001101100000',
         '0000011000110000';
       Initiales : array [1..3] of string[6] = 'LDJ', 'LDJ', 'LDJ';
       Resultats : array [1..3] of integer = 0, 0, 0;

{ Les variables globales du programme }

VAR Terrain : array [1..12, 1..15] of byte;

{ Les fonctions et procédures du programme }

  function IsLigne (y: byte) : boolean;
  var i: byte;
     begin
     result := true;
     for i := 2 to 11 do if Terrain[i, y] = 0 then result := false;
     end;

  function CanPutObject (x, y, n : byte) : boolean;
  var i, j: byte;
     begin
     result := true;
     for i := 1 to 4 do
       for j := 1 to 4 do
         if pieces[n][(j-1)*4+i]='1' and Terrain[x+i-1, y+j-1]=1 then result := false;
     end;

  procedure AfficheObject (x, y, n: byte);
  var i, j: byte;
     begin
     for i := 1 to 4 do
       for j := 1 to 4 do
         if pieces[n][(j-1)*4+i]='1' then putbloc ((x+i-1)*4, (y+j-1)*4, CarrePlein);
     end;

  procedure EffaceObject (x, y, n: byte);
  var i, j: byte;
     begin
     for i := 1 to 4 do
       for j := 1 to 4 do
         if pieces[n][(j-1)*4+i]='1' then putbloc ((x+i-1)*4, (y+j-1)*4, CarreVide);
     end;

  procedure MemoriseObject (x, y, n: byte);
  var i, j: byte;
     begin
     for i := 1 to 4 do
       for j := 1 to 4 do
         if pieces[n][(j-1)*4+i]='1' then terrain [x+i-1, y+j-1] := 1;
     end;

  procedure RotationObject (x, y, n: byte);
  var i, j: byte;
      a, b, c, d: char;
      rs: boolean;
     begin
     rs := true;
     for i := 1 to 4 do
       for j := 1 to 4 do
         if pieces[n][(i-1)*4+5-j]='1' and Terrain[x+i-1, y+j-1]=1 then rs := false;
     if rs then
       begin

       EffaceObject (x, y, n);

       a := pieces[n][6];
       pieces[n][6] := pieces[n][7];
       pieces[n][7] := pieces[n][11];
       pieces[n][11] := pieces[n][10];
       pieces[n][10] := a;

       a := pieces[n][1];
       b := pieces[n][2];
       c := pieces[n][3];
       d := pieces[n][4];
       for i := 1 to 4 do pieces[n][i] := pieces[n][4+(i-1)*4];
       for i := 1 to 4 do pieces[n][4+(i-1)*4] := pieces[n][17-i];
       for i := 1 to 4 do pieces[n][17-i] := pieces[n][13-(i-1)*4];
       pieces[n][13] := a;
       pieces[n][9] := b;
       pieces[n][5] := c;
       pieces[n][1] := d;

       AfficheObject (x, y, n);

       end;
     end;

  procedure AfficheAll;
  var i, j: byte;
      hdl: integer;
     begin
     for j := 14 downto 1 do
       for i := 2 to 11 do
         begin
         if terrain[i, j]=1 then hdl := CarrePlein else hdl := CarreVide;
         putbloc (i*4, j*4, hdl);
         end;
     end;

  procedure ScrollLine (l: byte);
  var i, j: byte;
     begin
     for j := l downto 2 do
       for i := 2 to 11 do
         terrain[i, j] := terrain[i, j-1];
     for i := 2 to 11 do terrain[i, 1] := 0;
     { AfficheAll; }
     end;

  var perdu: boolean;
      temps: integer;
      next, this: byte;
      score: integer;
      niveau: byte;

  procedure Jeu2;
  var x, y, n : byte;
      att, att2 : integer;
      ax : byte;
      flag: boolean;
    begin
    ax := 1;
    n := this;
    x := 2;
    y := 1;
    AfficheObject (x, y, n);
    flag:= false;
    if not CanPutObject (x, y, n) then begin perdu := true; flag := true; end;

    while not flag do
      begin
      for att := 1 to temps do
        begin
        if UpPressed then RotationObject (x, y, n);
        if RightPressed and CanPutObject (x+1, y, n) then ax := 2;
        if LeftPressed and CanPutObject (x-1, y, n) then ax := 0;
        if DownPressed then att := temps;
        if ax<>1 then
          begin
          EffaceObject (x, y, n);
          x := x+ax-1;
          AfficheObject (x, y, n);
          ax := 1;
          end;
        for att2 := 1 to 1000 do;
        end;
      if CanPutObject (x, y+1, n) then
        begin
        EffaceObject (x, y, n);
        y := y+ 1;
        AfficheObject (x, y, n);
        end 
        else flag := true;
      end;
      MemoriseObject (x, y, n);
    end;

  procedure AfficheNext;
  var i, j: byte;
      hdl: integer;
     begin
     for i := 1 to 4 do
       for j := 1 to 4 do
         begin
         if pieces[next][(j-1)*4+i]='1' then hdl := CarrePlein else hdl := CarreVide;
         PutBloc (80+(i-1)*4, 43+(j-1)*4, hdl);
         end;
     end;

  procedure Jeu;
  var j: byte;
      fl: boolean;
    begin
    niveau := 20;
    temps := 20;
    perdu := false;
    next := random(7)+1;
    while not perdu do 
      begin
      this := next;
      next := random(7)+1;
      AfficheNext;
      Jeu2;
      Score := Score+ 4;
      Niveau := Niveau- 1;
      if Niveau = 0 then
      begin
      Niveau := 15;
      if temps>2 then temps := temps- 1;
      Score := Score+ 100;
      end;
      fl := false;
      for j := 1 to 14 do 
        if IsLigne (j) then 
          begin
          fl := true;
          ScrollLine (j);
          end;
      if fl then AfficheAll;
      end;
    end;

  procedure Initialisation;
  var i, j: byte;
    begin
      for j := 1 to 14 do 
        begin
        for i := 2 to 11 do terrain [i, j] := 0;
        terrain [1, j] := 1;
        terrain [12, j] := 1;
        end;
      for i := 1 to 12 do terrain [i, 15] := 1;
    end;

{ Ici le programme principal }

var i, j: byte;
    ch: char;

BEGIN

score := 0;

while true do

  begin

  { le tableau des scores }

  PutBloc (0, 0, ScoresPicture);
  for i := 1 to 3 do
    begin
    if score > Resultats[i] then
      begin
        for j := 3 downto i+1 do 
          begin
          Initiales[j] := Initiales[j-1];
          Resultats[j] := Resultats[j-1];
          end;
        Resultats[i] := score;
        gotoxy ( 3, 4+i);
        Initiales[i] := LightRead (6);
        score := 0;
      end;
    gotoxy ( 3, 4+i); write (Initiales[i]);
    gotoxy (10, 4+i); write (Resultats[i]);
    end;

  while keypressed do ch := readkey;

  DisableInterrupt;

  while enterpressed or exitpressed do;
  while not enterpressed and not exitpressed do;

  { sortie du jeu }

  if ExitPressed then exit;

  { on joue }

  score := 0;
  Initialisation;
  PutBloc (0, 0, decor);
  AfficheAll;
  Jeu;

  EnableInterrupt;

  end;

END.