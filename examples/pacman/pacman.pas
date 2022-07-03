{ 
  Programme PACMAN pour HP
  Conçu à l'aide de HP PASCAL
  Copyright LDJ 1996
}

program Pacman;

uses gamehp, crtHp, SystemHp, Default;

ressource pacman;

var       
      fini: boolean;
      lost, won: boolean;
      nPG: byte;

      Terrain : array [1..13] of string;

      Sy : byte; 

      JState: byte;
      Jtx, Jty: integer;
      Jax, Jay: integer;
      Jx, Jy: integer;
      Jdec: integer;
      Jmvl, Jmvr, Jmvu, Jmvd:boolean;
      Jhdl: integer;

      Ftx1, Fty1:  integer;
      Fax1, Fay1: integer;
      Fx1, Fy1: integer;
      Fdec1: integer;
      Fmvl1,Fmvr1,Fmvu1,Fmvd1: boolean;

      Ftx2, Fty2:  integer;
      Fax2, Fay2: integer;
      Fx2, Fy2: integer;
      Fdec2: integer;
      Fmvl2,Fmvr2,Fmvu2,Fmvd2: boolean;

      Fhdl: integer;


procedure Affiche;
var x, y: byte;     
      hdl: integer;
begin
  for y:=1 to 13 do
    for x:=1 to 16 do
    begin
       case Terrain[y][x] of
           '8'   :  hdl := wsHorizontal;
           '6'   :  hdl := wsVertical;
           '7'   :  hdl := wdLeftDown;
           '1'   :  hdl := wdLeftUp;
           '3'   :  hdl := wdRightUp;
           '9'   :  hdl := wdRightDown;
           'u'   :  hdl := weUp;
           'd'   :  hdl := weDown;
           'r'   :  hdl := weRight;
           'L'   :  hdl := weLeft;
           'P'   :  hdl := pgMagic;
           '0'   :  hdl := pgSmall;
           '.'   :  hdl := vide;
       end;
       PutBloc ((x-1)*8, (y-1)*8, hdl);
    end;
end;

procedure AffDecor (x, y: integer);
var hdl:integer;
begin
    case Terrain[y][x] of
       'P'  :  hdl := pgMagic;
       '0'  :  hdl := pgSmall;
       '.'  :  hdl := vide;
     end;
     PutBloc ((x-1)*8, (y-1)*8, hdl);
end;

procedure GerePacMan;
var c: char;
begin
      if JDec = 0 then
      begin
        if LeftPressed and Jmvl then begin Jhdl := pmLeft; Jax := -1; Jay := 0; JDec := 8; end
        else if RightPressed and Jmvr then begin Jhdl := pmRight; Jax := 1; Jay := 0; JDec := 8; end
        else if UpPressed and Jmvu then begin Jhdl := pmUp; Jax := 0; Jay := -1; JDec := 8; end
        else if DownPressed and Jmvd then begin Jhdl := pmDown; Jax := 0; Jay := 1; JDec := 8; end
        else if Terrain[Jty+Jay][Jtx+Jax] in ['0', '.', 'P'] then JDec := 8 else begin Jax := 0; Jay := 0; end;
      end;
      PutBloc (Jx, Jy, vide);
      if Jax<>0 and Jdec <> 8 and Jdec <> 4 then PutBloc (Jx+4, Jy, vide);
      Jx := Jx + Jax;
      Jy := Jy + Jay;
      PutSprite (Jx, Jy, Jhdl);
      if JDec>0 then 
      begin
        JDec := JDec -1;      
        if JDec = 0 then
        begin
          Jtx := Jtx+Jax;
          Jty := Jty+Jay;
          c := Terrain[Jty][Jtx];
          if c in ['0', 'P'] then begin nPG:=nPG-1; if nPG=0 then won := true; end;
          if c = 'P' then begin JState := 150; FHdl := fProie; end;
          Terrain[Jty][Jtx] := '.';
          if Terrain[jty][jtx+1] in ['0', 'P', '.'] then Jmvr :=  true else Jmvr := false;
          if Terrain[jty][jtx-1]  in ['0', 'P', '.'] then Jmvl :=  true else Jmvl  := false;
          if Terrain[jty+1][jtx] in ['0', 'P', '.'] then Jmvd := true else Jmvd := false;
          if Terrain[jty-1][jtx]  in ['0', 'P', '.'] then Jmvu := true else Jmvu := false;
        end;
      end;
      if JState>0 then 
      begin
        JState := JState - 1;
        if JState = 0 then FHdl := fGlouton;
       end;
      if Jy>=32 then Sy := Jy-32;
      if Jy>73 then Sy := 41;
      SetScreenOffset (Sy);
end;


procedure GereFantome1;
begin
      if FDec1 = 0  then
       if JState=0 then
       begin
          if Jx<Fx1 and Fmvl1 then begin Fax1 := -1; Fay1 := 0; FDec1 := 9; end;
          if Jx>Fx1 and Fmvr1 then begin Fax1 := 1; Fay1 := 0; FDec1 := 9; end;
          if Jy<Fy1 and Fmvu1 then begin Fax1 := 0; Fay1 := -1; FDec1 := 9; end;
          if Jy>Fy1 and Fmvd1 then begin Fax1 := 0; Fay1 := 1; FDec1 := 9; end;
        end else
       begin
          if Jx>Fx1 and Fmvl1 then begin Fax1 := -1; Fay1 := 0; FDec1 := 9; end;
          if Jx<Fx1 and Fmvr1 then begin Fax1 := 1; Fay1 := 0; FDec1 := 9; end;
          if Jy>Fy1 and Fmvu1 then begin Fax1 := 0; Fay1 := -1; FDec1 := 9; end;
          if Jy<Fy1 and Fmvd1 then begin Fax1 := 0; Fay1 := 1; FDec1 := 9; end;
        end;
      if FDec1>0 then 
      begin
        FDec1 := FDec1 -1;
        if FDec1 = 0 then
        begin
            Ftx1 := Ftx1+Fax1;
            Fty1 := Fty1+Fay1;
            if               Terrain [Fty1+Fay1][Ftx1+Fax1] in ['0', 'P', '.']
               and not (Terrain [Fty1+Fax1][Ftx1+Fay1] in ['0', '.', 'P'])
               and not (Terrain [Fty1-Fax1][Ftx1-Fay1] in ['0', 'P', '.']) 
               then FDec1 := 8 else begin Fax1 :=0; Fay1:=0; end;
            if Terrain[Fty1][Ftx1+1] in ['0', '.', 'P'] then Fmvr1  := true else Fmvr1 := false;
            if Terrain[Fty1][Ftx1-1]  in ['0', '.', 'P'] then Fmvl1  := true else Fmvl1  := false;
            if Terrain[Fty1+1][Ftx1] in ['0', '.', 'P'] then Fmvd1 := true else Fmvd1 := false;
            if Terrain[Fty1-1][Ftx1]  in ['0', '.', 'P'] then Fmvu1 := true else Fmvu1 := false;
        end;
      end;
      if Fx1>Jx-7 and Fx1<Jx+7 and Fy1>Jy-7 and Fy1<Jy+7 and FHdl=fGlouton then lost := true;
      Fx1 := Fx1 + Fax1;
      Fy1 := Fy1 + Fay1;
      AffDecor (Ftx1, Fty1);
      AffDecor (Ftx1+Fax1, Fty1+Fay1);
      PutSprite (Fx1, Fy1, Fhdl);
end;


procedure GereFantome2;
begin
      if FDec2 = 0 then
        if JState=0 then
        begin
          if Jy<Fy2 and Fmvu2 then begin Fax2 := 0; Fay2 := -1; FDec2 := 9; end;
          if Jy>Fy2 and Fmvd2 then begin Fax2 := 0; Fay2 := 1; FDec2 := 9; end;
          if Jx<Fx2 and Fmvl2 then begin Fax2 := -1; Fay2 := 0; FDec2 := 9; end;
          if Jx>Fx2 and Fmvr2 then begin Fax2 := 1; Fay2 := 0; FDec2 := 9; end;
       end else
       begin
          if Jy>Fy2 and Fmvu2 then begin Fax2 := 0; Fay2 := -1; FDec2 := 9; end;
          if Jy<Fy2 and Fmvd2 then begin Fax2 := 0; Fay2 := 1; FDec2 := 9; end;
          if Jx>Fx2 and Fmvl2 then begin Fax2 := -1; Fay2 := 0; FDec2 := 9; end;
          if Jx<Fx2 and Fmvr2 then begin Fax2 := 1; Fay2 := 0; FDec2 := 9; end;
      end;
      if FDec2>0 then 
      begin
        FDec2 := FDec2 -1;
        if FDec2 = 0 then
        begin
            Ftx2 := Ftx2+Fax2;
            Fty2 := Fty2+Fay2;
            if              Terrain [Fty2+Fay2][Ftx2+Fax2] in ['0', 'P', '.'] 
              and not (Terrain [Fty2+Fax2][Ftx2+Fay2] in ['0', 'P', '.']) 
              and not (Terrain [Fty2-Fax2][Ftx2-Fay2] in ['0', 'P', '.']) 
              then FDec2 := 8 else begin Fax2 :=0; Fay2:=0; end;
            if Terrain[Fty2][Ftx2+1] in ['0', '.', 'P'] then Fmvr2  := true else Fmvr2 := false;
            if Terrain[Fty2][Ftx2-1]  in ['0', '.', 'P'] then Fmvl2  := true else Fmvl2  := false;
            if Terrain[Fty2+1][Ftx2] in ['0', '.', 'P'] then Fmvd2 := true else Fmvd2 := false;
            if Terrain[Fty2-1][Ftx2]  in ['0', '.', 'P'] then Fmvu2 := true else Fmvu2 := false;
        end;
      end;
     if Fx2>Jx-7 and Fx2<Jx+7 and Fy2>Jy-7 and Fy2<Jy+7 and fhdl=fGlouton then lost := true;
      Fx2 := Fx2 + Fax2;
      Fy2 := Fy2 + Fay2;
      AffDecor (Ftx2, Fty2);
      AffDecor (Ftx2+Fax2, Fty2+Fay2);
      PutSprite (Fx2, Fy2, Fhdl);
end;


procedure LoadLevel;
begin
  Terrain [ 1] := '7888888888888889';
  Terrain [ 2] := '6P000000000000P6';
  Terrain [ 3] := '60788r0L88r0L906';
  Terrain [ 4] := '606P00000000P606';
  Terrain [ 5] := '6060790790790606';
  Terrain [ 6] := '60d0130130130d06';
  Terrain [ 7] := '6000000000000006';
  Terrain [ 8] := '60u0790790790u06';
  Terrain [ 9] := '6060130130130606';
  Terrain [10] := '606P00000000P606';
  Terrain [11] := '60188r0L88r0L306';
  Terrain [12] := '6P000000000000P6';
  Terrain [13] := '1888888888888883';
  nPG := 98;

  Fx1 :=  64;
  Fy1 := 8;
  Ftx1 := 9;
  Fty1 := 2; 
  Fmvl1 := true;
  Fmvr1 := true;
  Fmvu1 := false;
  Fmvd1 := false;
  FDec1 := 0;
  Fax1 := 0;
  Fay1 := 0;

  Fx2 :=  8;
  Fy2 := 88;
  Ftx2 := 2;
  Fty2 := 12;
  Fmvl2 := true;
  Fmvr2 := true;
  Fmvu2 := false;
  Fmvd2 := false;
  FDec2 := 0;
  Fax2 := 0;
  Fay2 := 0;

  FHdl := fGlouton;

  JState := 0;
  Jx := 64;
  Jy := 48;      
  Jtx := 9;
  Jty := 7;
  Jmvl := true;
  Jmvr := true;
  Jmvu := false;
  Jmvd := false;
  JDec := 0;
  Jax := 0;
  Jay := 0;

  JHdl := pmRight;
end;



begin

  DisableInterrupt;

  while not fini do
  begin

    AppearCool (Presentation);  
    while not EnterPressed and not ExitPressed do;
    if ExitPressed then fini := true;
    DisappearCool;
    ClrScr;

    if fini then exit;

    LoadLevel;
    SetScreenOffset (16);
    Affiche;
    Won := false;
    Lost := false;

    while not lost and not won do
    begin
        GerePacMan;
        GereFantome1;
        GereFantome2;
    end;

    SetScreenOffset (0);

    DisAppearCool;
    if lost then AppearCool (Perdu);
    if won then AppearCool (Gagne);
    while not EnterPressed do;
    DisappearCool;

  end;

  EnableInterrupt;

end.

