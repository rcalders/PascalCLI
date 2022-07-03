{
  Programme INCENDIE pour HP
  Conçu à l'aide de HP PASCAL
  Copyright LDJ 1996
}

program Incendie;

uses Gamehp, crthp, default;

RESSOURCE incendie;

const
  xt : array [1..14] of byte = 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23;
  yt : array [1..14] of byte = 15, 18, 21, 18, 15, 18, 21, 18, 15, 18, 21, 18, 15, 18;
  ct : array [1..14] of byte =  3,  2,  1,  4,  3,  2,  1,  4,  3,  2,  1,  4,  3,  2;

procedure AffVictime (vx, vy, vnb: byte);
begin
 PutBloc (vx*5-8, vy*3-16, VictimeLeft+20*(vnb-1));
end;

procedure EffVictime (vx, vy: byte);
begin
  PutBloc (vx*5-8, vy*3-16, vide);
end;

procedure AffInfirmier (xi: byte);
begin
  PutBloc (xi*20+24, 55, Infirmier);
end;

procedure EffInfirmier (xi: byte);
begin
  PutBloc (xi*20+24, 55, VideInf);
end;

var i, j, n, p, s, t, scr : byte;
      ch: char;
      xlost: byte;
      pt : array [0..11] of byte;
      lost, encore : boolean;
      tps:integer;

begin

  n := 1;
  p := 1;
  s := 2;
  t := 0;
  lost := false;

  PutBloc (0, 0, Decor);

  AffInfirmier (p);

  while not lost do
  begin

    AffVictime (9, 17, 4);
    for i := 1 to n do pt[i] := 0;
    encore := true;

    while encore and not lost do 
    begin

      for i := 1 to n do 
      begin
        if i<>1 and pt[i-1]=4 then AffVictime (9, 13+(i mod 2)*4, 4);
        if (i=1 or pt[i-1]>5) and (pt[i]<=14) then
        begin
           pt[i] := pt[i]+ 1;
           if pt[i]=1 then EffVictime (9, 13+(i mod 2)*4);
           if pt[i] in [2..15] then EffVictime (xt[pt[i]-1], yt[pt[i]-1]);
           if pt[i] in [1..14] then AffVictime (xt[pt[i]], yt[pt[i]], ct[pt[i]]);
           if pt[i] = 15 then
           begin
              t := t+1;
              if t mod 50 = 0 then s := s+1;
           end;
           if yt[pt[i]]=21 then begin lost := true; xlost := xt[pt[i]]; end;
        end;
      end;

      
      for tps:=0 to 200-n*10 do
      begin
         if keypressed then ch := readkey else ch := ' ';
         if (p<3) and (ch=chr(26)) then begin EffInfirmier (p); p := p + 1; AffInfirmier (p); end;
         if (p>1) and (ch=chr(27)) then begin EffInfirmier (p); p := p - 1;  AffInfirmier (p); end;
         if p=xlost div 4-2 then lost := false;
       end;

      encore := false; 
      for j := 1 to n do if pt[j]<15 then encore := true;

    end;

    n := n + 1;
    if n>18 then n := 18;

  end;

  while not keypressed do;

end.