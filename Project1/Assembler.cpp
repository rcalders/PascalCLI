// Assembler.cpp: implementation of the CAssembler clAss.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Assembler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAssembler::CAssembler(CLdjTextFile *Fichier, CHPPASCALCompilerCtrl *dialogue)
{
	dlg=dialogue;

	// Définition des constantes

	Version48 = 'M';
	Version49 = 'C';
	
	ST3[0] = "RSI RTN RTI P== ?P# ?P= P=C C=P HEX ASC NUL ";
	ST3[1] = "A=0 B=0 C=0 D=0 A=B B=C C=A D=C B=A C=B A=C ";
	ST3[2] = "C=D ASL BSL CSL DSL ASR BSR CSR DSR ";
	
	COD3[0] = "80810";
	COD3[1] = "01";
	COD3[2] = "0F";
	COD3[3] = "2";
	COD3[4] = "88";
	COD3[5] = "89";
	COD3[6] = "80D";
	COD3[7] = "80C";

	ST4[0] = "REL2 REL3 REL4 REL5 ?A=B ?B=C ?C=A ?D=C ?A#B ?B#C ?C#A ";
	ST4[1] = "?D#C ?A=0 ?B=0 ?C=0 ?D=0 ?A#0 ?B#0 ?C#0 ?D#0 ?A>B ?B>C ";
	ST4[2] = "?C>A ?D>C ?A<B ?B<C ?C<A ?D<C ABEX BCEX CAEX DCEX A=-A ";
	ST4[3] = "B=-B C=-C D=-D CPEX HS=0 ST=0 ST=1 R0=A R1=A R2=A R3=A ";
	ST4[4] = "R4=A R0=C R1=C R2=C R3=C R4=C A=R0 A=R1 A=R2 A=R3 A=R4 ";
	ST4[5] = "C=R0 C=R1 C=R2 C=R3 C=R4 D0=A D1=A D0=C D1=C A=IN C=IN ";
	ST4[6] = "C=ID ASLC BSLC CSLC DSLC ASRC BSRC CSRC DSRC ASRB BSRB ";
	ST4[7] = "CSRB DSRB XM=0 SB=0 SR=0 MP=0 PC=A PC=C A=PC C=PC C=ST ";
	ST4[8] = "ST=C RTNC NOP3 NOP4 NOP5 ";

	ST4bis = "000102030408090A0B0C101112131418191A1B1C30313435";
	
	ST4ter = "02030610111213141516171C1D1E1F21222428";

	COD4[0] = "80F";
	COD4[1] = "82";
	COD4[2] = "84";
	COD4[3] = "85";
	COD4[4] = "09";
	COD4[5] = "0A";
	COD4[6] = "400";
	COD4[7] = "420";
	COD4[8] = "6300";
	COD4[9] = "64000";
	
	ST5[0] = "A=A+B B=B+C C=C+A D=D+C A=A+A B=B+B C=C+C D=D+D B=A+B ";
	ST5[1] = "C=B+C A=C+A C=D+C A=A-1 B=B-1 C=C-1 D=D-1 A=A-B B=B-C ";
	ST5[2] = "C=C-A D=D-C A=A+1 B=B+1 C=C+1 D=D+1 B=B-A C=C-B A=A-C ";
	ST5[3] = "C=C-D A=B-A B=C-B C=A-C D=C-D A=A&B B=B&C C=C&A D=D&C ";
	ST5[4] = "B=A&B C=B&C A=C&A C=D&C A=A!B B=B!C C=C!A D=D!C B=A!B ";
	ST5[5] = "C=B!C A=C!A C=D!C ?A>=B ?B>=C ?C>=A ?D>=C ?A<=B ?B<=C ";
	ST5[6] = "?C<=A ?D<=C ?HS=0 ?ST=0 ?ST=1 RTNSC RTNCC CLRST CSTEX ";
	ST5[7] = "P=P+1 P=P-1 AR0EX AR1EX AR2EX AR3EX AR4EX CR0EX CR1EX ";
	ST5[8] = "CR2EX CR3EX CR4EX AD0EX AD1EX CD0EX CD1EX D0=AS D1=AS ";
	ST5[9] = "AD0XS AD1XS D0=CS D1=CS CD0XS CD1XS OUT=C C+P+1 RESET ";
	ST5[10] = "BUSCC SREQ? ?XM=0 ?SB=0 ?SR=0 ?MP=0 INTON BUSCB BUSCD ";
	ST5[11] = "APCEX CPCES RTNNC ";

	ST5bis = "367238BCD0123489ABC236789ABCDEF19ABE124803D67";

	ST6[0] = "D0=(2) D0=(4) D0=(5) D1=(2) D1=(4) D1=(5) A=-A-1 B=-B-1 ";
	ST6[1] = "C=-C-1 D=-D-1 ASRB.F BSRB.F CSRB.F DSRB.F A=R0.F A=R1.F ";
	ST6[2] = "A=R2.F A=R3.F A=R4.F C=R0.F C=R1.F C=R2.F C=R3.F C=R4.F ";
	ST6[3] = "R0=A.F R1=A.F R2=A.F R3=A.F R4=A.F R0=C.F R1=C.F R2=C.F ";
	ST6[4] = "R3=C.F R4=C.F DAT0=A DAT1=A A=DAT0 A=DAT1 DAT0=C DAT1=C ";
	ST6[5] = "C=DAT0 C=DAT1 D0=D0+ D1=D1+ D0=D0- D1=D1- ABIT=0 ABIT=1 ";
	ST6[6] = "CBIT=0 CBIT=1 RTNSXM SETHEX SETDEC RSTK=C C=RSTK OUT=CS ";
	ST6[7] = "UNCNFG CONFIG SHUTDN PC=(A) PC=(C) INTOFF CLRHST ";
	
	ST6bis ="9ABDEF678C4589045670457CEF";

	ST56 = "A=A+ B=B+ C=C+ D=D+ A=A- B=B- C=C- D=D- ";
	
	ST7[0] = "?ABIT=0 ?ABIT=1 ?CBIT=0 ?CBIT=1 AR0EX.F AR1EX.F AR2EX.F ";
	ST7[1] = "AR3EX.F AR4EX.F CR0EX.F CR1EX.F CR2EX.F CR3EX.F CR4EX.F ";
	
	LSTR = "LAREL LCREL LAABS LCABS ";
	
	CHAMP[0] = "A";
	CHAMP[1] = "P";
	CHAMP[2] = "WP";
	CHAMP[3] = "XS";
	CHAMP[4] = "X";
	CHAMP[5] = "S";
	CHAMP[6] = "M";
	CHAMP[7] = "B";
	CHAMP[8] = "W";	
	
	JP1[0] = "GOC";
	JP1[1] = "GONC";
	JP1[2] = "GOTO";
	JP1[3] = "GOLONG";
	JP1[4] = "GOSUB";
	JP1[5] = "GOSUBL";
	JP1[6] = "GOVLNG";
	JP1[7] = "GOSBVL";
	JP1[8] = "GOINA";
	JP1[9] = "GOINC";
	
	JP2[0] = "4";
	JP2[1] = "5";
	JP2[2] = "6";
	JP2[3] = "8C";
	JP2[4] = "7";
	JP2[5] = "8E";
	JP2[6] = "8D";
	JP2[7] = "8F";
	JP2[8] = "80824";
	JP2[9] = "34";

	// Initialisation de l'assemblage

	numline = 0;
	count = 0;
	test = false;
	Erreur = false;
	labels.RemoveAll ();

	if (dlg->calculatrice==HP48) result = (CString)"HPHP48-" + Version48;
	else if (dlg->calculatrice==HP49) result = (CString)"HPHP49-" + Version49;
	
	ConvertAscii (result);
	Ajouter (result);

	result = (CString)"CCD2000000";
	Ajouter (result);

	// Assemblage

	Fichier->StartReading();
	int size=Fichier->GetLength();		

	try
	{
		int count = 0;
		while (!Fichier->eof())
		{
			CString chaine = "";
			char octet=0;
			while (true)
			{
				octet = Fichier->Read();
				count++;
				if ((count%25)==0) 
				{
					dlg->EtapeJauge(count*100/size);
					dlg->Update ();
				}
				if (octet=='\r') break;
				chaine+=octet;
			}
			AssLine (chaine);
			octet=Fichier->Read();
		}
		dlg->EtapeJauge(100);
		dlg->Update ();
	}
	catch(CString err)
	{
	}
	catch(char*)
	{
	}	
}


CAssembler::~CAssembler()
{

	// Cloture de l'assemblage

	LineNul = true; result = "";
}


//////////////////////////////////////////////////////////////////////
// Fonctions auxiliaires
//////////////////////////////////////////////////////////////////////

char CAssembler::Valeur (char k)
{     
	if ((k>='0') && (k<='9')) return (k-48);
	if ((k>='A') && (k<='F')) return (k-55);
	throw "Erreur interne !";
}

char CAssembler::Car (char k)
{
	if (k<=9) return (48+k);
	if ((k>=10) && (k<=15)) return (55+k);
	throw "Erreur interne !";   
}

CString CAssembler::Adr (int n)
{
     CString st = "00000";
	 const int MaxLongInt = 1048576;
     if (n<0) n += MaxLongInt;
     for (int i=1; i<=5; i++)
     {
          st.SetAt(i-1, Car(n%16));
          n = n/16;
     }
     return (st);
}

void CAssembler::ConvertAscii (CString& st)
{
	CString st2 = "";	
	int n = st.GetLength();
	if (n>127) { DoErreur (10); return; }
	for (int i=1; i<=2*n; i++) st2 += ' ';
	for (int i=1; i<=n; i++)
	{
		int nb = st[i-1];
		st2.SetAt(2*i-2, Car(nb%16));
		st2.SetAt(2*i-1, Car(nb/16));
	}
	st = st2;
}

bool CAssembler::OkHexa (CString& st)
{
	if (st!="") if ((st[0]=='#') || (st[0]=='$')) st = st.Right(st.GetLength()-1);
	bool test = (st!="");
	for (int i=1; i<=st.GetLength(); i++)
		test = test && 
		(
		((st[i-1]>='0') && (st[i-1]<='9')) ||
		((st[i-1]>='A') && (st[i-1]<='F'))
		);
	return (test);  
}

void CAssembler::Val (CString chaine, int& resultat, int& erreur)
{
	wchar_t * stopstring;
	
	if (chaine[0] == '$')
		resultat = wcstoul(chaine.Right(chaine.GetLength() - 1), &stopstring, 16);
//		resultat = strtol ((const char*)chaine.Right(chaine.GetLength()-1).GetBuffer(), &stopstring, 16);
	else
		resultat = wcstoul(chaine, &stopstring, 10);
		//resultat = strtol((const char *)chaine.GetBuffer(), &stopstring, 10);
	erreur = errno;
}

int CAssembler::Pos (CString contenu, CString conteneur)
{
	return (conteneur.Find(contenu)+1);
}

void CAssembler::DoErreur (int n)
{
	CString mess;
	switch (n)
	{
          case 1: mess="Instruction inconnue"; break;
          case 2: mess="Erreur de syntaxe"; break;
          case 3: mess="Nom déjà utilisé"; break;
          case 4: mess="Argument incorrect ou absent"; break;
          case 5: mess="Saut trop long"; break;
          case 6: mess="CHAMP incorrect ou absent"; break;
          case 7: mess="Nom incorrect"; break;
          case 8: mess="Link incorrect"; break;
          case 9: mess="RTN/GOYES attendu"; break;
          case 10: mess="Argument trop long"; break;		  
     }
     ErreurInfos.Message = mess+" dans \""+(CString) linebis+"\"";
	 Erreur = true;
     throw "Erreur !";
}


//////////////////////////////////////////////////////////////////////
// Fonctions de bas-niveau : contrôle des labels ...
//////////////////////////////////////////////////////////////////////

bool CAssembler::OkName (CString& st)
{
	if (st=="") return false;
	return ( 
		((st[0]>='A') && (st[0]<='Z')) ||
		((st[0]>='a') && (st[0]<='z')) ||
		(st[0]=='@')
		);
}   

bool CAssembler::Duplic (CString& st)
{
	POSITION position = labels.GetHeadPosition();
	if (position==NULL) return false;
	while (true)
	{
		ptr = labels.GetAt(position);

		if (ptr.name==st) return (true);
		if (position==labels.GetTailPosition()) return (false);

		labels.GetNext(position);
	}
}

bool CAssembler::OkEtiq ()
{
	POSITION position = labels.GetHeadPosition ();
	if (position==NULL) return true;
	while (true)
	{
		ptr = labels.GetAt (position);
		if (ptr.typ==0)	return (false);
		if (position==labels.GetTailPosition()) return (true);
		labels.GetNext(position);
	}
}

void CAssembler::Etiq ()
{
	str2 = str1;
	str1 = str1.Right(str1.GetLength()-1);

	if (!(OkName (str1)))
		DoErreur(7);
	else
	{
		if (Duplic(str1))
		{
			if (ptr.typ>0)
			{
				DoErreur (3);
				return;
			}
		}
		ReplEtiq();
		ptr.name = str1;
		ptr.typ = 1;
		ptr.cont = count;		
		labels.AddTail (ptr);
	}	
	str1=Item(line);
}

//////////////////////////////////////////////////////////////////////
// Fonctions de bas-niveau : gestion des Adresses des labels ...
//////////////////////////////////////////////////////////////////////

int CAssembler::Argument ()
{
	if (Duplic (str2))
	{
		if (ptr.typ<2) 
		{
			DoErreur (4);
			return -1;
		}
		else 
			return (ptr.cont);
	}
	else
	{
		if (str2!="") if (str2[0]=='#') str2.SetAt(0, '$');
		int err, n; Val (str2, n, err);
		if (err>0)
		{
			DoErreur(2); 
			return -1;
		}
		else 
			return (n);
	}	
}

CString CAssembler::Offset (int n, int a1, int a2)
{	
	int max, ofs, dest;
	
	if (Duplic (str2))
	{
		if (ptr.typ!=0)
		{
			if (ptr.typ==2)
				DoErreur (3);
			else
			{
				dest = ptr.cont;
				ofs = dest-a2;
				max = 1 << (4*n-1);
				if (abs(ofs)>=max) 
					DoErreur (5); 
				else 
					return (Adr(ofs).Left (n));
			}			
		}
	}
	
	if (!(OkName (str2))) 
	{
		DoErreur (7);
		return "";
	}
	else
	{		
		ptr.name = str2;
		ptr.typ = 0;
		ptr.cont = n;
		ptr.adr1 = a1;
		ptr.adr2 = a2;
		labels.AddTail (ptr);
		return ( ((CString)"?????").Left(n));
	}
}

//////////////////////////////////////////////////////////////////////
// Fonctions de gestion des entrées - sorties
//////////////////////////////////////////////////////////////////////

CString CAssembler::Item (CString& line)
{
     CString st = "";	 
     while (line!="") { if ((line[0]==9) || (line[0]==32)) line = line.Right(line.GetLength()-1); else break; }
     if (line!="")
	 {
		 if (line[0]==34)
		 {
			 while (true)
			 {
				 st+=line[0];
				 line = line.Right(line.GetLength()-1);
				 if (line=="") break;
				 if (line[0]==34) break;
			 }
             line = line.Right(line.GetLength()-1);
             st+="\"";
		 }
		 else
		 {
			 while (true)
			 {
				 st+=line[0];
				 line = line.Right(line.GetLength()-1);
				 if (line=="") break;
				 if ((line[0]==9) || (line[0]==32)) break;
			 }
		 }
	 } 
	 return (st);
}


void CAssembler::AjouterBack (int ad, int n, CString st)
{
  for (int i=0; i<=n-1; i++) tab[ad+i]=st[i];
}


void CAssembler::Ajouter (CString& st)
{
	for (int i=1; i<=st.GetLength(); i++)
		tab[count++] = st[i-1];
}

//////////////////////////////////////////////////////////////////////
// Compilateur
//////////////////////////////////////////////////////////////////////

void CAssembler::IfTest ()
{
	test = false;
	if (str1=="GOYES")
		result = Offset (2, count, count);
	else 
		if (str1=="RTNYES") 
			result = "00";
		else 
			DoErreur (9);
}

void CAssembler::ReplEtiq ()
{	
	POSITION position = labels.GetTailPosition ();
	if (position==NULL) return;
	while (true)
	{
		POSITION last = position;
		if (position!=labels.GetHeadPosition()) ptr = labels.GetPrev (position); else ptr=labels.GetAt(position);
		if (ptr.name==str1)
		{
			int max = 1 << (4*ptr.cont-1);
			int offs = count-ptr.adr2;
			if (offs>=max) 
				DoErreur(5); 
			else 
				AjouterBack (ptr.adr1, ptr.cont, Adr(offs));
			labels.RemoveAt(last);
		}
		if (position==last) break;
	}
}

void CAssembler::Equate ()
{
	str1 = str1.Right(str1.GetLength()-1);
	if (!(OkName(str1))) DoErreur (7);
	if (Duplic(str1)) DoErreur (3);
	if (str2!="") if (str2[0]=='#') str2.SetAt(0, '$');
	int err, n;
	Val (str2, n, err);
	if (err!=0)
		DoErreur (2);
	else
	{
		ptr.name = str1;
		ptr.typ = 2;
		ptr.cont = n;
		labels.AddTail (ptr);
	}
}


void CAssembler::Saut ()
{
	int p=0;
	int ad;
	while ((p<8) && (str1!=JP1[p])) p++;
	if (p==8)
		DoErreur (1);
	else
	{
		result = JP2[p];
		if (p<8) ad = count+result.GetLength();
		if (p==0) p = 1;
		if ((p>=1) && (p<=3)) result = result + Offset (p+1, ad, ad);
		if ((p>=4) && (p<=5)) result = result + Offset (p-1, ad, ad+p-1);
		if ((p>=6) && (p<=7)) result = result + Adr (Argument());
    }
}

char CAssembler::Nibble (CString& st, bool flag)
{
	int n = 0, err = 0;
	if (st!="") if (st[0]=='#') st.SetAt(0, '$');
	Val (st, n, err);
	if (err!=0)
	{
		DoErreur (2);
		return -1;
	}
	else
	{
		if (flag)
		{
			flag = ((n>=1) && (n<=16));
			n--;
		} 
		else flag = ((n>=0) && (n<=15));
		if (!flag)
		{
			DoErreur (4);
			return -1;
		}
		else 
			return (Car (n));
	}	
}


void CAssembler::LoadCst ()
{
	CString st;
	int i, n;	
	bool fl;

	st = str1.Left(3);
	if (st=="LA(") 
		result = "8082";
	else 
	{
		if (st=="LC(") 
			result = "3";
		else 
			DoErreur (1);
	}
	
	n = str1.GetLength();
	if ((n<5) || (str1[n-1]!=')'))
		DoErreur (1);
	else
	{
		str1 = str1.Mid(3, n-4);
		result = result + Nibble (str1, true);
		int k, err;
		Val (str1, k, err);	
		fl = false;
		
		if (str2[0]==34)
		{
			fl = true;
			n = str2.GetLength();
			n = n-2;
			if (n>8) n=8;
			str2 = str2.Mid(1, n);
			for (i=n; i<=7; i++) str2+=" ";
			ConvertAscii (str2);
			result += str2.Left(k);
		}
		
		if ((OkHexa(str2)) && (!fl))
		{
			n = str2.GetLength();
			for (i=n; i<=15; i++) str2 = (CString)"0"+str2;
			for (i=1; i<=k; i++) result += str2[16-i];
		} 
		else 
			DoErreur (2);
	}
}

void CAssembler::LoadAC ()
{
	int p = Pos (str1, LSTR);
	switch (p)
	{
	case 1: result = (CString)"80824"+ Offset (5, count+5, count); break;
	case 7: result = (CString)"34"+ Offset (5, count+2, count); break;
	case 13: result = (CString)"200681B580824"+ Offset (5, count+13, count+8)+ (CString)"CA07"; break;
	case 19: result = (CString)"20DE0681B580824"+ Offset (5, count+15, count+10)+ (CString)"CA07DE"; break;
	default: LoadCst ();
	}
}

void CAssembler::Field (CString stA, CString stB, CString st)
{
  int i = 0;
  while ((i<9) && (str2!=CHAMP[i])) i++;
  if ((i==7) && (stB!="")) 
	  result = stB;
  else
  {
	  switch (i) 
	  {
      case 0 : result = stA; break;
      case 9 : DoErreur (6); break;
      default :		  
		  int j = Pos ((CString)"b", st);
		  if (j==0) j=Pos((CString)"a", st); else i += 8;
		  st.SetAt(j-1, Car (i-1));
		  result = st;		  
	  }
  }
}

void CAssembler::Div3 (int p)
{	
	switch (p)
	{
		
    case 8 : if (OkHexa (str2)) result = str2; else DoErreur (2); break;
    case 9 : 
		if (str2[0]==34) str2 = str2.Mid (1, str2.GetLength()-2);
		ConvertAscii (str2);
		result = str2;
		break;
	case 10 :
		result = "";
		int n = Argument ();
		if (n>255)
			DoErreur(2);
		else for (int i=1; i<=n; i++) result += '0';
		break;
	}	
}

void CAssembler::Ass3()
{
  int i = 0;
  int p = 0;
  char c;
  while ((i<3) && (p==0)) p = Pos (str1, ST3[i++]);
  if (p==0) 
	  DoErreur (1);
  else
  {
	  p = (p/4)+11*(i-1);    
      if ((p>=0) && (p<=2)) result = COD3[p];
      if ((p>=3) && (p<=7)) result = COD3[p]+Nibble(str2, false);
      if ((p>=8) && (p<=10)) Div3(p);
      if ((p>=11) && (p<=22)) { c = Car(p-11); Field ((CString)"D"+c,"",(CString)"Ab"+c); }
      if ((p>=23) && (p<=30)) { c = Car(p-23); Field ((CString)"F"+c,"",(CString)"Bb"+c); }
  }
}

void CAssembler::Ass4()
{
	int i = 0;
	int p = 0;
	char c;
	while ((i<9) && (p==0)) p = Pos (str1, ST4[i++]);
	if (p == 0) 
		DoErreur (1);
	else
	{
		p = (p/5)+11*(i-1);
		if ((p>=0) && (p<=3)) result = Offset (p+2, count, count);
		if ((p>=4) && (p<=19)) { c = Car(p-4); Field ((CString)"8A"+c,"",(CString)"9a"+c); }
		if ((p>=20) && (p<=27)) { c = Car(p-20); Field ((CString)"8B"+c,"",(CString)"9b"+c); }
		if ((p>=28) && (p<=31)) { c = Car(p-16); Field ((CString)"D"+c,"",(CString)"Ab"+c); }
		if ((p>=32) && (p<=35)) { c = Car(p-24); Field ((CString)"F"+c,"",(CString)"Bb"+c); }
		if ((p>=36) && (p<=39)) result = COD4[p-36]+Nibble(str2, false);
		if ((p>=40) && (p<=63)) result = (CString)"1"+ST4bis.Mid(2*p-79-1, 2);
		if ((p>=64) && (p<=82)) result = (CString)"8"+ST4ter.Mid(2*p-127-1, 2);
		if ((p>=83) && (p<=86)) result = (CString)"81B"+Car(p-81);
		if ((p>=87) && (p<=92)) result = (CString)COD4[p-83];
    }
}

void CAssembler::Ass56()
{
	char c, d;
	int p = Pos(str1.Left(4), ST56);
	if (p==0) 
		DoErreur (1); 
	else
	{
		p = p / 5;
		c = Car ((p%4)+8*(p/4));
		str1 = str1.Mid (4, 255);
		d = Nibble (str1, true);
		Field (((CString)"818F"+c)+d,"",((CString)"818a"+c)+d);
	}
}

void CAssembler::Ass5()
{
     int i = 0; int p = 0; char c;

	 while ((i<12) && (p==0))  p = Pos (str1, ST5[i++]);

     if (p==0) 
		 Ass56 (); 
	 else
     {
		 p =(p/6)+9*(i-1);
		 if ((p>=0) && (p<=15))
		 {
			 c = Car (p);
			 Field ((CString)"C"+c,"",(CString)"Aa"+c);
		 }
		 if ((p>=16) && (p<=31))
		 {
			 c = Car (p-16);
			 Field ((CString)"E"+c,"",(CString)"Ba"+c);
		 }
		 if ((p>=32) && (p<=47))
		 {
			 c = Car (p-32);
			 Field ((CString)"0EF"+c,"",(CString)"0Ea"+c);
		 }
		 if ((p>=48) && (p<=55))
		 {
			 c = Car (p-40);
			 Field ((CString)"8B"+c,"",(CString)"9b"+c);
		 }
		 if ((p>=56) && (p<=58)) result = (CString)"8"+ST5bis[p-56]+Nibble(str2, false);
		 if ((p>=59) && (p<=64)) result = (CString)"0"+ST5bis[p-56];
		 if ((p>=65) && (p<=74)) result = (CString)"12"+ST5bis[p-56];
		 if ((p>=75) && (p<=86)) result = (CString)"13"+ST5bis[p-56];
		 if ((p>=87) && (p<=91)) result = (CString)"80"+ST5bis[p-56];
		 if ((p>=92) && (p<=95)) result = (CString)"83"+ST5bis[p-56];
		 if ((p>=96) && (p<=98)) result = (CString)"808"+ST5bis[p-56];
		 if ((p>=99) && (p<=100)) result = (CString)"81B"+ST5bis[p-56];
		 if (p==101) result = (CString)"500";          
     }
}

void CAssembler::LoadD (int p)
{
	char n;
	switch (p)
	{		
	case 0 : case 3 : n = 2; break;
	case 1 : case 4 : n = 4; break;
    case 2 : case 5 : n = 5; break;
	}
    result = (CString)"1"+ST6bis[p]+Adr(Argument()).Left(n);
}

void CAssembler::Ass6 ()
{
     int i = 0;
     int p = 0;
	 char c, d;
     while ((i<8) && (p==0)) p = Pos (str1, ST6[i++]);
     if (p==0) 
		 Ass56 ();
	 else
     {
          p = (p/7)+8*(i-1);
		  if ((p>=0) && (p<=5)) LoadD (p);
		  if ((p>=6) && (p<=9)) 
		  { 
			  c = Car (p+6);
			  Field ((CString)"F"+c,"",(CString)"Bb"+c);
		  }
		  if ((p>=10) && (p<=13)) 
		  {
			  c = Car (p-10);
			  Field ((CString)"819F"+c,"",(CString)"819a"+c);
		  }
		  if ((p>=14) && (p<=23)) 
		  {
			  p -= 14;
			  c = Car (8*(p/5)+(p%5));
			  Field ((CString)"81AF1"+c,"",(CString)"81Aa1"+c);
		  }
		  if ((p>=24) && (p<=33)) 
		  {
			  p -= 24;
			  c = Car (8*(p/5)+(p%5));
			  Field ((CString)"81AF0"+c,"",(CString)"81Aa0"+c);
		  }
		  if ((p>=34) && (p<=41)) 
		  {
			  p -= 34;
			  i = 0;
			  c = Car (p);
			  d = Car (p+8);
			  while ((i<9) && (str2!=CHAMP[i])) i++;
			  if (i<9) 
				  Field ((CString)"14"+c,(CString)"14"+d,((CString)"15"+c)+'a');
			  else 
				  result = (CString)"15"+d+Nibble(str2, true);
		  }
          if ((p>=42) && (p<=45)) result = (CString)"1"+ST6bis[p-36]+Nibble (str2, true);
		  if ((p>=46) && (p<=49)) result = (CString)"808"+ST6bis[p-36]+Nibble (str2, false);
		  if ((p>=50) && (p<=54)) result = (CString)"0"+ST6bis [p-36];
		  if ((p>=55) && (p<=58)) result = (CString)"80"+ST6bis [p-36];
		  if ((p>=59) && (p<=61)) result = (CString)"808"+ST6bis [p-36];
		  if (p==62) result = (CString)"82F";          
     }
}

void CAssembler::Ass7()
{
     int i = 0;
     int p = 0;	 
     while ((i<2) && (p==0)) p = Pos (str1, ST7[i++]);
     if (p==0) 
		 DoErreur (1);
	 else
     {
		 p = p/8+7*(i-1);
		 if ((p>=0) && (p<=3)) result = (CString)"808"+((CString)"67AB").Mid(p,1)+Nibble(str2, false);
		 if ((p>=4) && (p<=13))
		 {
			 p -= 4;
			 char c = Car (8*(p/5)+(p%5));
			 Field ((CString)"81AF2"+c,"",(CString)"81Aa2"+c);
		 }
	 }
}

void CAssembler::Assembleur(CString& str1, CString& str2)
{
	result = "";
	if (str1=="*ASM_DEBUG:FICHIER") ErreurInfos.Fichier=str2;
	if (str1=="*ASM_DEBUG:LIGNE") ErreurInfos.Ligne=StrToInt(str2);
	LineNul = ((str1=="") || (str1[0]=='*'));
	if (!LineNul)
	{
        if (test) 
			IfTest ();
		else
		{
			if (str1=="RESSOURCE") { MergeRessource(); return; }
			if (str1.Left(2)=="==") { Equate (); return; }
			switch (str1[0])
			{            
			case 'G' : Saut (); break;
			case 'L' : LoadAC (); break;
			default:				
				if (str1[0]=='?') test = true;
				switch (str1.GetLength())
				{
				case 2: case 3: Ass3(); break;
				case 4: Ass4(); break;
				case 5: Ass5(); break;
				case 6: Ass6(); break;
				case 7: Ass7(); break;
				default: DoErreur (1);
				}                
			}
		}
	}
}	


void CAssembler::Assembler_Ligne()
{	
	numline ++;
	ok = true;
	linebis = line;
	str1 = Item (line);
	str1.MakeUpper();
	if (str1!="") if (str1[0]==':') Etiq ();
	if (str1!="ASC") str2.MakeUpper();
	str2 = Item(line);
	Assembleur (str1, str2);
	if (!Erreur) Ajouter (result);
}


//////////////////////////////////////////////////////////////////////
// Communication avec HP - PASCAL
//////////////////////////////////////////////////////////////////////

void CAssembler::AssLine (CString Ligne)
{
	line = Ligne;	
	if (line != "") Assembler_Ligne ();
}


void CAssembler::StopAssLine ()
{	
	CString nom;
	
	if ((!OkEtiq()) && (!Erreur))
	{
		Erreur = true;
		nom = ptr.name;          
		ErreurInfos.Message = "Label " + nom + " appelée et non déclarée ";		
	}
	
	if (!Erreur) 
	{
		// Termine Tab
		CString valor = "00";		// RC
		Ajouter (valor);
		if (count%2==0)
		{
			AjouterBack (21, 5, Adr(count-21));
			tab[count++] = 0;
		}
		else
		{
			AjouterBack (21, 5, Adr(count-21-1));
		}
		
		// Enregistre sur disque
		
		CFile FichierSortie (dlg->directoryDestination()+dlg->fileDestination(), CFile::modeCreate | CFile::modeWrite);
		tab[count++]=0;
		tab[count]=0;
		int j=0;
		char buffer[1024];
		for (int i=0; i<count/2-1;i++)
		{
			buffer[j]=Valeur(tab[2*i])+16*Valeur(tab[2*i+1]);
			if (++j==1024) 
			{ 
				FichierSortie.Write(buffer,1024);
				j=0; 
			}
		}
		if (j!=0) FichierSortie.Write(buffer,j);		
		FichierSortie.Close();
	}
	
}

// --------------------------
// Incorporation de ressource
// --------------------------

void CAssembler::MergeRessource ()
{
	CFile fic (dlg->directoryResource()+str2, CFile::modeRead);

	while (true)
	{
		char taillenom;
		fic.Read (&taillenom, 1);
		if (taillenom==0) break;
		fic.Seek(16+2+149, CFile::current);
	}

	fic.Seek(167, CFile::current);
	
	int taille = fic.Read(tab+count, 128000-count);
	count+=taille;

	fic.Close();
}