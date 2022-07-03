// MacroCompile.cpp: implementation of the CMacroCompile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MacroCompile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CMacroCompile::Chargement(CLdjTextFile *Fichier)
{
	// Chargement du fichier source
	Fichier->StartReading();

	// Initialisation de la liste d'instructions
	ListeInstructions.RemoveAll();

	int size = Fichier->GetLength();
	int ofs=0;

	// Lecture du fichier
	while (!Fichier->eof())
	{
		// Lit une chaine
		CString chaine = "";
		char octet = 0;
		while (octet!='\r')
		{
			octet = Fichier->Read();
			ofs++;
			if (ofs%50==0) 
			{
				dlg->EtapeJauge(ofs*50/size);
				dlg->Update();
			}
			if (octet=='\r') break;
			chaine+=octet;
		}

		//	Remplit les champs de TInstruction
		TInstruction current;
		current.op=chaine;
		current.arg1="";
		current.arg2="";
		current.arg3="";
		current.arg4="";

		chaine += ":";
		if (chaine.Find((CString)":")!=-1) { current.op=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (current.op=="DATA") { current.arg1=chaine.Left(chaine.GetLength()-1); chaine = "";}
		
		if (chaine.Find((CString)":")!=-1) { current.arg1=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (chaine.Find((CString)":")!=-1) { current.arg2=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (chaine.Find((CString)":")!=-1) { current.arg3=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }
		if (chaine.Find((CString)":")!=-1) { current.arg4=chaine.Left(chaine.Find((CString)":")); chaine=chaine.Right(chaine.GetLength()-chaine.Find((CString)":")-1); }

		ListeInstructions.AddTail (current);

		// On passe à la ligne suivante
		octet=Fichier->Read();
		ofs++;
		if (ofs%50==0) 
		{
			dlg->EtapeJauge(ofs*50/size);
			dlg->Update();
		}
	}
}

CMacroCompile::CMacroCompile(CLdjTextFile * Fichier, CHPPASCALCompilerCtrl *dialogue)
{
	dlg = dialogue;

	// Chargement du fichier source
	Chargement (Fichier);

	// Création du fichier de sortie
	generatedFile = new CLdjTextFile (dlg->directoryDestination(), "Resultat4.txt");

	// Effectue la traduction
	count=9;
	FonctionsHard.RemoveAll();
	Traduction ();
}

CMacroCompile::~CMacroCompile()
{
}

void CMacroCompile::AppelFonctionHard (CString nom)
{
	if ((nom!="@FH@OP_REAL_NEG") && (nom.GetLength()>=12))
		if (nom.Left(12)=="@FH@OP_REAL_") 
		{
			if (FonctionsHard.Find((CString)"@FH@OP_REAL_BINAIRE_START")==NULL)
				FonctionsHard.AddTail((CString)"@FH@OP_REAL_BINAIRE_START");
			if (FonctionsHard.Find((CString)"@FH@OP_REAL_BINAIRE_END")==NULL)
				FonctionsHard.AddTail((CString)"@FH@OP_REAL_BINAIRE_END");
		}

	if ((nom=="@FH@OP_REAL_SUPERIOR") || (nom=="@FH@OP_REAL_INFERIOR"))
	{
		if (FonctionsHard.Find((CString)"@FH@OP_REAL_SOUSTRACTION")==NULL)
			FonctionsHard.AddTail((CString)"@FH@OP_REAL_SOUSTRACTION");
	}

	if (FonctionsHard.Find(nom, FonctionsHard.GetHeadPosition())==NULL)
		FonctionsHard.AddTail(nom);
}

void CMacroCompile::SendNoOptimize(CString chaine)
{
	if (OptFlag) { EndSending (); OptFlag=false;}

	if (chaine.GetLength()>=11) if (chaine.Left(11)=="GOSUBL @FH@") AppelFonctionHard (chaine.Right(chaine.GetLength()-7));
	chaine += "\r\n";
	generatedFile->Write(chaine);
}

void CMacroCompile::Send(CString chaine)
{
	OptFlag = true;

	// Place la chaine en fin de buffer

	buf[9]=chaine;

	// Optimisations
	// -------------

	while (true)
	{
		if (count==9) break;

		// Mvts de D0
		if ((buf[8].Left(6)=="D0=D0+") && (buf[9].Left(6)=="D0=D0-"))
		{
			for (int i=9; i>=count+2; i--) buf[i]=buf[i-2];
			count +=2;
			continue;
		}
		
		// LA + C=A et LC + A=C
		if ((buf[8].Left(3)=="LA(") && (buf[9].Left(4)=="C=A "))
		{
			buf[9]=(CString)"LC("+(buf[8]).Right(buf[8].GetLength()-3);
			for (int i=8; i>=count+1; i--) buf[i]=buf[i-1];
			count +=1;
			continue;
		}
		
		if ((buf[8].Left(3)=="LC(") && (buf[9].Left(4)=="A=C "))
		{
			buf[9]=(CString)"LA("+buf[8].Right(buf[8].GetLength()-3);
			for (int i=8; i>=count+1; i--) buf[i]=buf[i-1];
			count +=1;
			continue;
		}

		if ((buf[8].Left(3)=="A=0") && (buf[9].Left(4)=="C=A "))
		{
			buf[9]=(CString)"C=0"+buf[8].Right(buf[8].GetLength()-3);
			for (int i=8; i>=count+1; i--) buf[i]=buf[i-1];
			count +=1;
			continue;
		}

		if ((buf[8].Left(3)=="C=0") && (buf[9].Left(4)=="A=C "))
		{
			buf[9]=(CString)"A=0"+buf[8].Right(buf[8].GetLength()-3);
			for (int i=8; i>=count+1; i--) buf[i]=buf[i-1];
			count +=1;
			continue;
		}
		
		// Usage inutile de la pile
		if (((buf[8].Left(6)=="DAT0=A") && (buf[9].Left(6)=="A=DAT0"))
			|| ((buf[8].Left(6)=="DAT0=C") && (buf[9].Left(6)=="C=DAT0")))
		{
			for (int i=9; i>=count+2; i--) buf[i]=buf[i-2];
			count +=2;
			continue;
		}
		
		if ((buf[8].Left(6)=="DAT0=A") && (buf[9].Left(6)=="C=DAT0"))
		{
			buf[9]=(CString)"C=A "+(buf[8]).GetAt(7);
			for (int i=8; i>=count+1; i--) buf[i]=buf[i-1];
			count +=1;
			continue;
		}
		
		if ((buf[8].Left(6)=="DAT0=C") && (buf[9].Left(6)=="A=DAT0"))
		{
			buf[9]=(CString)"A=C "+(buf[8]).GetAt(7);
			for (int i=8; i>=count+1; i--) buf[i]=buf[i-1];
			count +=1;
			continue;
		}

		// echange les niveaux 8 et 9 pour d'autres optim
		
		if (count<=7)
			if (
				((buf[8].Left(3)=="LA(") || (buf[8].Left(3)=="LC(") || (buf[8].Left(3)=="C=0") || (buf[8].Left(3)=="A=0")) 
				&& (buf[7].Left(6)=="D0=D0+")
				&& (buf[9].Left(6)=="D0=D0-")
				)
		{
			CString tmp=buf[7];
			buf[7]=buf[8];
			buf[8]=tmp;
			continue;
		}
		
		if (count<=7)
			if (
				((buf[8].Left(3)=="LA(") || (buf[8].Left(3)=="A=0")) 
				&& (buf[7].Left(6)=="DAT0=C")
				&& (buf[9].Left(6)=="C=DAT0")
				)
		{
			CString tmp=buf[7];
			buf[7]=buf[8];
			buf[8]=tmp;
			continue;
		}

		if (count<=7)
			if (
				((buf[8].Left(3)=="LC(") || (buf[8].Left(3)=="C=0")) 
				&& (buf[7].Left(6)=="DAT0=A")
				&& (buf[9].Left(6)=="A=DAT0")
				)
		{
			CString tmp=buf[7];
			buf[7]=buf[8];
			buf[8]=tmp;
			continue;
		}

		// Plus d'optimisation possbile

		break;		
	}

	// Envoie la chaine la plus ancienne
	// ---------------------------------

	if (count==0)
	{
		chaine = buf[0];

		if (chaine.GetLength()>=11) if (chaine.Left(11)=="GOSUBL @FH@") AppelFonctionHard (chaine.Right(chaine.GetLength()-7));
		
		chaine += "\r\n";
		generatedFile->Write(chaine);
		count=1;
	}

	// Rotation des éléments du buffer

	for (int i=count-1; i<9; i++) buf[i]=buf[i+1];
	count--;
}

void CMacroCompile::EndSending ()
{
	for (int i=count; i<9; i++)
	{
		CString chaine = buf[i];

		if (chaine.GetLength()>=11) if (chaine.Left(11)=="GOSUBL @FH@") AppelFonctionHard (chaine.Right(chaine.GetLength()-7));
		
		chaine += "\r\n";
		generatedFile->Write(chaine);
	}
	count = 9;
}

// --------------------------------
// Macrocompilation proprement dite
// --------------------------------

void CMacroCompile::Traduction ()
{
	// Point d'entrée du programme

	Send ("GOLONG @INIT");

	int size = ListeInstructions.GetCount();
	int count = 0;

	POSITION position = ListeInstructions.GetHeadPosition();
	TInstruction current;

	while (true)
	{
		current = ListeInstructions.GetAt(position);

		// Directives de compilation
		if (current.op=="MAIN") { Send("* Début du programme"); Send (":@START"); }
		if (current.op=="END") TraduitEND();
		if (current.op=="UNIT") Send("* Début de l'unité "+current.arg1+" *");
		if (current.op=="ENDUNIT") { Send("* Fin de l'unité *"); Send (""); }
		if (current.op=="PROC") TraduitProc (current);
		if (current.op=="ENDPROC") TraduitEndProc (current);

		// Gestion des remarques
		if (current.op=="REM") SendNoOptimize ("*ASM_DEBUG:"+current.arg1);
		// Gestion de la directive ASM
		if (current.op=="ASM") 
			SendNoOptimize (current.arg1);


		// Gestion des étiquettes
		if (current.op=="LABEL") 
			Send (":"+current.arg1);

		// Constantes, ressources ...
		if (current.op=="CONST") { typCONST = current.arg1; if (typCONST=="STRING") typCONST2 = StrToInt(current.arg2.GetString()); }
		if (current.op=="DATA") TraduitCONST(current);

		// Contrôle du flux
		if (current.op=="GOTO") Send ("GOTO "+current.arg1);
		if (current.op=="GOLONG") Send ("GOLONG "+current.arg1);
		if (current.op=="HALT") Send ("GOLONG @END");

		if (current.op=="GOYES") TraduitSautConditionnel(current);
		if (current.op=="GONO") TraduitSautConditionnel(current);
		if (current.op=="GO_IF_EQUAL") TraduitSautConditionnel(current);
		if (current.op=="GO_IF_IN") TraduitSautConditionnel(current);

		// Gestion des procédures et foncitons
		if (current.op=="CALL") Send ("GOSUBL @F@"+current.arg1);
		if (current.op=="RTN") Send ("GOLONG @F@"+ProcName+"@END");

		// Gestion des affectations de variables
		if (current.op=="LOAD") TraduitLOAD(current);
		if (current.op=="SAVE") TraduitSAVE(current);

		// Gestion du typage
		if (current.op=="CV") TraduitCV(current);

		// Opérations directes sur la pile pour évaluer des expressions
		if (current.op=="CST") TraduitCST(current);
		if (current.op=="OP") TraduitOP(current);

		// Multiplications et divisions par des constantes
		if (current.op=="HARDMULT") TraduitHARDMULT(current);

		// Gestion de la pile assembleur
		if (current.op=="DUP") TraduitDUP(current);
		if (current.op=="DROP") TraduitDROP(current);

		if (position==ListeInstructions.GetTailPosition()) break;

		ListeInstructions.GetNext(position);

		count++;
		if (count%25==0) 
		{
			dlg->EtapeJauge(50+count*50/size);
			dlg->Update ();
		}
	}

	EndSending ();
}

void CMacroCompile::TraduitCONST(CMacroCompile::TInstruction current)
{
	// Ressources

	if (typCONST=="RESSOURCE") SendNoOptimize ("RESSOURCE "+current.arg1);

	// Constantes typées

	if (typCONST=="BYTE") SendNoOptimize ("HEX "+ReverseHexa(HexaChar(StrToInt(current.arg1.GetString()))));
	if (typCONST=="INTEGER") SendNoOptimize ("HEX "+ReverseHexa(HexaInteger(StrToInt(current.arg1.GetString()))));
	if (typCONST=="REAL") SendNoOptimize ("HEX "+ReverseHexa(HexaFloat(StrToFloat(current.arg1.GetString()))));

	// Chaines de caractères

	if (typCONST=="STRING") 
	{
		SendNoOptimize ("HEX "+ReverseHexa(HexaChar(current.arg1.GetLength())));
		SendNoOptimize ("ASC \""+current.arg1+"\"");
		SendNoOptimize ("NUL "+IntToStr(2*(typCONST2-current.arg1.GetLength())));
	}
}

void CMacroCompile::TraduitProc (CMacroCompile::TInstruction current)
{
	// Initiation de la procédure
	ProcName = current.arg1;
	SendNoOptimize ("* Début de la procédure "+ProcName+" *");
	SendNoOptimize (":@F@"+ProcName);

     // Mis à jour de R1 et D0
     SendNoOptimize ("AD0EX");     
	 SendNoOptimize ("LC(5) "+HexaInteger(StrToInt(current.arg2.GetString())+StrToInt(current.arg3.GetString())));
     SendNoOptimize ("A=A-C A");
	 SendNoOptimize ("C=R1.F A");
	 SendNoOptimize ("D=C A");
     SendNoOptimize ("R1=A.F A");	 
	 SendNoOptimize ("LC(5) "+HexaInteger(StrToInt(current.arg2.GetString())+StrToInt(current.arg3.GetString())+StrToInt(current.arg4.GetString())));
	 SendNoOptimize ("A=C+A A");
	 SendNoOptimize ("D0=A");

     // Sauvegarde R1
     SendNoOptimize ("C=D A");
     SendNoOptimize ("DAT0=C A");
     SendNoOptimize ("D0=D0+ 5");

	 // Sauvegarde nouvel R1+Taille du paramètre
	 SendNoOptimize ("LA(5) "+HexaInteger(StrToInt(current.arg2.GetString())));
	 SendNoOptimize ("C=R1.F A");
	 SendNoOptimize ("A=C+A A");
	 SendNoOptimize ("DAT0=A A");
     SendNoOptimize ("D0=D0+ 5");

     // Sauve l'adresse retour 
     SendNoOptimize ("C=RSTK");
     SendNoOptimize ("DAT0=C A");
     SendNoOptimize ("D0=D0+ 5");
}

void CMacroCompile::TraduitEndProc (CMacroCompile::TInstruction current)
{
	// Label de fin de procédure
	SendNoOptimize (":@F@"+ProcName+"@END");

	// Récupère l'adresse retour
	SendNoOptimize ("D0=D0- 5");
	SendNoOptimize ("C=DAT0 A");
	SendNoOptimize ("RSTK=C");
	
	// Récupère l'adesse où doit se trouver D0
	SendNoOptimize ("D0=D0- 5");
	SendNoOptimize ("C=DAT0 A");

	// Récupère l'adesse des variables locales
	SendNoOptimize ("D0=D0- 5");
	SendNoOptimize ("A=DAT0 A");
	SendNoOptimize ("R1=A");

	// Place D0 où il faut
	SendNoOptimize ("D0=C");
	
	// Retour à l'appelant
	SendNoOptimize ("RTN");
}

void CMacroCompile::TraduitSautConditionnel(CMacroCompile::TInstruction current)
{
	if (current.op=="GO_IF_EQUAL")
	{
		Depile ("C", current.arg1);
		Depile ("A", current.arg1);
		Empile ("A", current.arg1);
		Send ((CString)"?C=A "+Champ(current.arg1));
		Send ("GOYES "+current.arg2);
	}

	if (current.op=="GO_IF_IN")
	{
		Depile ("C", current.arg1);
		Depile ("A", current.arg1);
		Send ((CString)"B=C "+Champ(current.arg1));
		Depile ("C", current.arg1);
		Empile ("C", current.arg1);
		Send ((CString)"B=B-A "+Champ(current.arg1));
		Send ((CString)"C=C-A "+Champ(current.arg1));
		Send ((CString)"?B>=C "+Champ(current.arg1));
		Send ("GOYES "+current.arg2);
	}

	if ((current.op=="GOYES") || (current.op=="GONO"))
	{
		Depile ("A", "BOOLEAN");
		if (current.op=="GONO") Send ("?A=0 P");
		if (current.op=="GOYES") Send ("?A#0 P");
		Send ("GOYES "+current.arg1);
	}
}

void CMacroCompile::TraduitLOAD(CMacroCompile::TInstruction current)
{
	if (current.arg1=="STRING")
	{
		Send ("GOSUBL @FH@LOAD_STRING");
	}
	else
	{
		Depile ("A", "INTEGER");
		Send ("D1=A");
		LectureMemoire ("A", current.arg1);
		Empile ("A", current.arg1);
	}
}

void CMacroCompile::TraduitSAVE(CMacroCompile::TInstruction current)
{
	if (current.arg1=="STRING")
	{
		Send ("GOSUBL @FH@SAVE_STRING");
	}
	else
	{
		Depile ("C", current.arg1);
		Depile ("A", "INTEGER");
		Send ("D1=A");
		EcritureMemoire ("C", current.arg1);
	}
}

void CMacroCompile::TraduitCV(CMacroCompile::TInstruction current)
{
	// Gestion des conversions entre nombres

	if ((current.arg1=="BYTE") && (current.arg2=="INTEGER"))
	{ 
		Send ("A=0 A"); 
		Depile ("A", "BYTE"); 
		Empile ("A", "INTEGER");
	}

	if ((current.arg1=="INTEGER") && (current.arg2=="BYTE"))
	{ 
		Depile ("A", "INTEGER");
		Empile ("A", "BYTE");
	}

	// Entiers vers reels ou chaine de caracteres

	if ((current.arg1=="BYTE") || (current.arg1=="INTEGER"))
	{
		if (current.arg2=="REAL")
		{
			if (current.arg1=="BYTE") Send ("A=0 A");
			Depile ("A", current.arg1);
			Send ("GOSUBL @FH@CV_INTEGER_REAL");
			Empile ("A", "REAL");
		}
		if (current.arg2=="STRING")
		{
//			if (current.arg1=="BYTE") Send ("A=0 A");
			if (current.arg1=="BYTE") { Send ("A=3 A"); Send ("A=0 A"); }
			Depile ("A", current.arg1);
			Send ("GOSUBL @FH@CV_INTEGER_STRING");
		}
	}

	// Réels vers entiers ou chaines de caractère

	if (current.arg1=="REAL") 
	{ 
		if ((current.arg2=="INTEGER")  || (current.arg2=="BYTE"))
		{
			Send ("GOSUBL @FH@CV_REAL_INTEGER");
			Empile ("A", current.arg2);
		}		
		if (current.arg2=="STRING") 
		{
			Depile ("A", "REAL");
			Send ("GOSUBL @FH@CV_REAL_STRING");
		}
	}

	// Caractère vers chaine de caractere

	if ((current.arg1=="CHAR") && (current.arg2=="STRING"))
		Send ("GOSUBL @FH@CV_CHAR_STRING");

	// Chaine de caractère vers caractère

	if ((current.arg1=="STRING") && (current.arg2=="CHAR"))
	{
		Send ("AD0EX");
		Send ("LC(5) #FE");
		Send ("A=A-C A");
		Send ("D0=A");
		Send ("A=DAT0 B");
		Send ("D0=D0- 2");
		Send ("DAT0=A B");
		Send ("D0=D0+ 2");
	}

	// Chaine de caractère vers entier ou reel

	if (current.arg1=="STRING")
	{
		if ((current.arg2=="BYTE") || (current.arg2=="INTEGER"))
		{
			Send ("GOSUBL @FH@CV_STRING_INTEGER");
			Empile ("A", current.arg2);
		}
		if (current.arg2=="REAL") 
		{
			Send ("GOSUBL @FH@CV_STRING_REAL");
			Empile ("A", "REAL");
		}
	}
}

void CMacroCompile::TraduitCST(CMacroCompile::TInstruction current)
{
	if (current.arg1=="GLOBAL")
	{
		Send ("A=R2");
		Send ("LC(5) "+HexaInteger(StrToInt(current.arg2.GetString())));
		Send ("A=C+A A");
		Empile ("A", "INTEGER");
		return;
	}

	if (current.arg1=="LOCAL")
	{
		Send ("A=R1");
		Send ("LC(5) "+HexaInteger(StrToInt(current.arg2.GetString())));
		Send ("A=C+A A");
		Empile ("A", "INTEGER");
		return;
	}

	if ((current.arg1=="CONST") || (current.arg1=="STATIC"))
	{
		Send ("A=R4");
		Send ("LC(5) "+HexaInteger(StrToInt(current.arg2)));
		Send ("A=C+A A");
		Empile ("A", "INTEGER");
		return;
	}

	if (current.arg1=="STRING")
	{
		Send ("AD0EX");
		Send ("LC(5) #100");
		Send ("A=C+A A");
		Send ("D0=A");
		return;
	}

	if (current.arg1=="BYTE") Send ("LA(2) "+HexaChar(StrToInt(current.arg2.GetString())));
	if (current.arg1=="INTEGER") Send ("LA(5) "+HexaInteger(StrToInt(current.arg2.GetString())));
	if (current.arg1=="REAL") Send ("LA(16) "+HexaFloat(StrToFloat(current.arg2.GetString())));
	if (current.arg1=="BOOLEAN") if (current.arg2=="FALSE") Send ("A=0 P"); else Send("LA(1) 1");
	Empile ("A", current.arg1);
}

void CMacroCompile::TraduitOP(CMacroCompile::TInstruction current)
{
	// Opérateur unaire

	if (current.arg2=="NEG")
	{
		Depile ("A", current.arg1);
		if ((current.arg1=="BYTE") || (current.arg1=="INTEGER")) Send ((CString)"A=-A "+Champ(current.arg1));
		if (current.arg1=="REAL") { Send ("A=-A S"); Send ("A=A+1 S"); }
		if (current.arg1=="BOOLEAN") Send ("A=A+1 P");
		Empile ("A", current.arg1);
	}

	if (current.arg2=="LENGTH")
	{
		Send ("AD0EX");
		Send ("LC(5) #FE");
		Send ("A=A-C A");
		Send ("D0=A");
	}

	// Additions et soustractions

	if ((current.arg2=="+") || (current.arg2=="-"))
	{
		// Bytes et Integers
		if ((current.arg1=="BYTE") || (current.arg1=="INTEGER"))
		{
			Depile ("C", current.arg1);
			Depile ("A", current.arg1);
			if (current.arg2=="+") Send ((CString)"A=C+A "+Champ(current.arg1));
			if (current.arg2=="-") Send ((CString)"A=A-C "+Champ(current.arg1));
			Empile ("A", current.arg1);
		}

		// Les réels
		if (current.arg1=="REAL") 
		{
			if (current.arg2=="+") Send ("GOSUBL @FH@OP_REAL_ADDITION");
			if (current.arg2=="-") Send ("GOSUBL @FH@OP_REAL_SOUSTRACTION");
		}

		// Les string
		if (current.arg1=="STRING") Send ("GOSUBL @FH@OP_STRING_ADDITION");
	}


	// Multiplications et divisions

	if (current.arg2=="*") 
	{
		if (current.arg1=="BYTE")
		{			
			Send ("A=0 A");
			Send ("C=0 A");
			Depile ("C", "BYTE");
			Depile ("A", "BYTE");
//			Send ("GOSBVL #53EE4");	// 53EE4  2709E   MPY
			Send ("GOSBVL #2709E");
			Empile ("A", "BYTE");
		}
		if (current.arg1=="INTEGER")
		{
			Depile ("C", "INTEGER");
			Depile ("A", "INTEGER");
//			Send ("GOSBVL #53EE4");	// 53EE4  2709E   MPY
			Send ("GOSBVL #2709E");
			Empile ("A", "INTEGER");
		}
		if (current.arg1=="REAL") Send ("GOSUBL @FH@OP_REAL_MULTIPLICATION");
	}
	if ((current.arg2=="/") || (current.arg2=="DIV")) Send ("GOSUBL @FH@OP_"+current.arg1+"_DIVISION");
	if (current.arg2=="MOD") Send ("GOSUBL @FH@OP_"+current.arg1+"_MODULO");


	// Opération spécifique sur les STRING
	
	if (current.arg2=="COPY") Send ("GOSUBL @FH@OP_STRING_COPY");

	// Opérations sur les BOOLEAN
	
	if ((current.arg2=="AND") || (current.arg2=="OR"))
	{
		Depile ("A", "BOOLEAN");
		Depile ("C", "BOOLEAN");
		if (current.arg2=="AND") Send ("A=C&A P");
		if (current.arg2=="OR") Send ("A=C!A P");
		Empile ("A", "BOOLEAN");
	}


	// Génération de BOOLEAN

	if ((current.arg2=="=") || (current.arg2=="<>"))
	{
		if (current.arg1=="STRING")
		{
			Send ("@FH@OP_STRING_EQUAL");
			if (current.arg2=="<>")
			{
				Depile ("A", "BOOLEAN");
				Send ("A=-A P");
				Send ("A=A+1 P");
				Empile ("A", "BOOLEAN");
			}
		}
		else
		{
			Depile ("C", current.arg1);
			Depile ("A", current.arg1);
			Send ((CString)"C=C-A "+Champ(current.arg1));
			Send ("A=0 P");
			if (current.arg2=="<>") Send ((CString)"?C=0 "+Champ(current.arg1));
			if (current.arg2=="=") Send ((CString)"?C#0 "+Champ(current.arg1));
			CString lab = NextLabel ();
			Send ("GOYES "+lab);
			Send ("A=A+1 P");
			Send (":"+lab);
			Empile ("A", "BOOLEAN");
		}
	}

	if ((current.arg2=="<") || (current.arg2==">=") || (current.arg2=="=>"))
	{		
		if (current.arg1=="STRING") Send ("GOSUBL @FH@OP_STRING_INFERIOR");
		if (current.arg1=="REAL") Send ("GOSUBL @FH@OP_REAL_INFERIOR");
		if (current.arg1=="INTEGER")
		{
			Depile ("C", "INTEGER");
			Depile ("A", "INTEGER");
			Send ("A=A-C A");
			Send ("LC(5) #70000");
			Send ("?C>A A");
			CString lab = NextLabel ();
			Send ("GOYES "+lab);
			Send ("C=C+1 P");
			Send (":"+lab);
			Empile ("C", "BOOLEAN");			
		}
		if (current.arg1=="BYTE")
		{
			Send ("C=0 A");
			Send ("A=0 A");
			Depile ("C", "BYTE");
			Depile ("A", "BYTE");
			Send ("A=A-C A");
			Send ("LC(5) #70000");
			Send ("?C>A A");
			CString lab = NextLabel ();
			Send ("GOYES "+lab);
			Send ("C=C+1 P");
			Send (":"+lab);
			Empile ("C", "BOOLEAN");
		}
		if (current.arg2!="<") 
		{
			Depile ("A", "BOOLEAN");
			Send ("A=-A P");
			Send ("A=A+1 P");
			Empile ("A", "BOOLEAN");
		}
	}

	if ((current.arg2==">") || (current.arg2=="<=") || (current.arg2=="=<"))
	{		
		if (current.arg1=="STRING") Send ("GOSUBL @FH@OP_STRING_SUPERIOR");
		if (current.arg1=="REAL") Send ("GOSUBL @FH@OP_REAL_SUPERIOR");
		if (current.arg1=="INTEGER")
		{
			Depile ("C", "INTEGER");
			Depile ("A", "INTEGER");
			Send ("C=C-A A");
			Send ("LA(5) #70000");
			Send ("?C<A A");			
			CString lab = NextLabel();
			Send ("GOYES "+lab);
			Send ("A=A+1 P");
			Send (":"+lab);
			Empile ("A", "BOOLEAN");
		}
		if (current.arg1=="BYTE")
		{
			Send ("C=0 A");
			Send ("A=0 A");
			Depile ("C", "BYTE");
			Depile ("A", "BYTE");
			Send ("C=C-A A");
			Send ("LA(5) #70000");
			Send ("?C<A A");			
			CString lab = NextLabel();
			Send ("GOYES "+lab);
			Send ("A=A+1 P");
			Send (":"+lab);
			Empile ("A", "BOOLEAN");
		}
		if (current.arg2!=">")
		{
			Depile ("A", "BOOLEAN");
			Send ("A=-A P");
			Send ("A=A+1 P");
			Empile ("A", "BOOLEAN");
		}
	}
}


void CMacroCompile::TraduitHARDMULT(CMacroCompile::TInstruction current)
{
	Depile ("A", current.arg1);

	int Nb = StrToInt (current.arg2.GetString());

    int CtDec = 0;

	while ((Nb%2)!=1)
	{
	  CtDec++;
	  Nb /= 2;
	}
	 
	if ((Nb%2)==1)
	{
		while (CtDec >= 4)
		{                    
			CtDec -= 4;
			Send ((CString)"ASL "+Champ(current.arg1));                    
		}
		while (CtDec > 0)
		{
			CtDec--;
			Send ((CString)"A=A+A "+Champ(current.arg1));
		}
	}

	Nb/=2;

	if (Nb != 0) Send ((CString)"C=A "+Champ(current.arg1));
	
	while (Nb != 0)
	{               
		CtDec = 1;
		while ((Nb % 2)!=1)
		{
			CtDec++;
			Nb/=2;
		}
		if ((Nb % 2)==1)
		{
			while (CtDec >= 4)
			{
				CtDec -= 4;
				Send ((CString)"CSL "+Champ(current.arg1));
			}		
			while (CtDec > 0)
			{
				CtDec--;
				Send ((CString)"C=C+C "+Champ(current.arg1));
			}
		}

		Send ((CString)"A=C+A "+Champ(current.arg1));
		Nb/=2;
	}
	Empile ("A", current.arg1);
}


void CMacroCompile::TraduitDUP(CMacroCompile::TInstruction current)
{
	Depile ("A", current.arg1);
	Empile ("A", current.arg1);
	Empile ("A", current.arg1);
}

void CMacroCompile::TraduitDROP(CMacroCompile::TInstruction current)
{
	Send ("D0=D0- "+IntToStr(Size(current.arg1)));
}

//////////////////////////////////////////////////////////////////////
// Fonctions d'initialisation et de cloture du contexte HPPASCAL
//////////////////////////////////////////////////////////////////////

void CMacroCompile::TraduitEND()
{
	// Tout d'abord on écrit le retour au RPL
	// --------------------------------------

	// Retour au RPL
	
	SendNoOptimize (":@END");
	
	// Rend l'ancien contexte écran
	
	SendNoOptimize ("LA(5) #8068D");
	SendNoOptimize ("D0=A");
	SendNoOptimize ("A=DAT0 A");
	SendNoOptimize ("LC(5) #120");
	SendNoOptimize ("D0=C");
	SendNoOptimize ("DAT0=A A");
	
	// Affiche la zone des menus
	SendNoOptimize ("LA(5) #8069A");
	SendNoOptimize ("D0=A");
	SendNoOptimize ("A=DAT0 B");
	SendNoOptimize ("LC(5) #128");
	SendNoOptimize ("D0=C");
	SendNoOptimize ("DAT0=A B");
	
	// Récupère les registres
	
//	SendNoOptimize ("GOSBVL #67D2");
	
	// ON C puis retour au RPL

//	SendNoOptimize ("GOVLNG #2D564");	 
	
	if (dlg->calculatrice==HP48) SendNoOptimize ("GOSBVL #00D57");	//Vide le buffer clavier : Flush
	else if (dlg->calculatrice==HP49) SendNoOptimize ("GOSBVL #267A6");
	SendNoOptimize ("GOVLNG #05143");	//GETPTRLOOP

	// Ici on introduit les fonctions HARD utilisées
	// ---------------------------------------------

	
		// Chargement du fichier source
	CString fichier = "";
	if (dlg->calculatrice==HP48) fichier="StdInclude.txt";
	else if (dlg->calculatrice==HP49) fichier="StdInclude_49.txt";
	
	CFile FichierEntree (dlg->directoryUnit()+fichier, CFile::modeRead);
	
		// Lecture du fichier

		bool flag = false;
		while (FichierEntree.GetPosition()!=FichierEntree.GetLength())
		{		
			CString chaine = "";
			char octet = 0;			
			while (octet!='\r')
			{
				FichierEntree.Read(&octet, 1);
				if (octet=='\r') break;
				chaine+=octet;
			}
			FichierEntree.Read(&octet, 1);
			if (chaine==":@FH@END") flag = false;
			if (chaine!="") if ((chaine[0]==':') && (FonctionsHard.Find(chaine.Right(chaine.GetLength()-1))!=NULL)) flag=true;
			if (flag) SendNoOptimize (chaine);
		}			

		// Fermeture du fichier source

		FichierEntree.Close();
	

	// Maintenant on écrit l'initialisation
	// -------------------------------------

	SendNoOptimize (":@INIT");
	
	// Conneries qui peut être influent
	SendNoOptimize ("SETHEX");
	SendNoOptimize ("P= 0");
	
	// Sauvegarde tous les registres
	SendNoOptimize ("GOSBVL #679B");
	
	SendNoOptimize ("AD1EX");
	
	// Les variables globales
	SendNoOptimize ("LC(5) "+ HexaInteger(dlg->memoryGlobal));
	SendNoOptimize ("A=A-C A");
	SendNoOptimize ("R2=A");
	SendNoOptimize ("D1=A");          // Initialise à 0
	SendNoOptimize ("GOSBVL #675C");
	SendNoOptimize ("A=R2");
	
	// La mémoire écran
	SendNoOptimize ("LC(5) "+ HexaInteger(dlg->memoryBITMAP()*2+132));
	SendNoOptimize ("A=A-C A");
	SendNoOptimize ("LC(5) #FFFFC");
	SendNoOptimize ("A=C&A A");
	SendNoOptimize ("R3=A");                  
	SendNoOptimize ("LC(5) "+ HexaInteger(dlg->memoryBITMAP()*2+132));
	SendNoOptimize ("D1=A");          // Initialise à 0
	SendNoOptimize ("GOSBVL #675C");
	SendNoOptimize ("A=R3");
	
	// Assigne le Crt sur la zone réservée
	SendNoOptimize ("LC(5) #120");
	SendNoOptimize ("D1=C");
	SendNoOptimize ("DAT1=A A");
	
	SendNoOptimize ("B=A A");
	
	// Marge à gauche à 0
	SendNoOptimize ("LC(5) #100");
	SendNoOptimize ("D1=C");
	SendNoOptimize ("LC(1) 8");
	SendNoOptimize ("DAT1=C P");
	
	// Supprime la zone des menus
	SendNoOptimize ("LC(5) #8069A");
	SendNoOptimize ("D0=C");
	SendNoOptimize ("A=DAT0 B");
	SendNoOptimize ("LC(2) #C0");
	SendNoOptimize ("A=C&A B");
	SendNoOptimize ("LC(2) #3F");
	SendNoOptimize ("A=C!A B");
	SendNoOptimize ("LC(5) #128");
	SendNoOptimize ("D0=C");
	SendNoOptimize ("DAT0=A B");
	
	SendNoOptimize ("A=B A");
	
	// La pile
	SendNoOptimize ("LC(5) "+ HexaInteger(dlg->memorySTACK()*2+dlg->memoryRPL()*2+4));
	SendNoOptimize ("A=A-C A");
	SendNoOptimize ("D0=A");
	
	// Variables locales et statiques
	SendNoOptimize ("LAABS @ENDCODE");
	SendNoOptimize ("R4=A");
	SendNoOptimize ("GOLONG @START");
	SendNoOptimize (":@ENDCODE");
}

//////////////////////////////////////////////////////////////////////
// Fonctions auxiliaires
//////////////////////////////////////////////////////////////////////

void CMacroCompile::Depile (CString registre, CString datatype)
{
	Send ("D0=D0- "+IntToStr(Size(datatype)));
	Send (registre+"=DAT0 "+Champ(datatype));	
}

void CMacroCompile::Empile (CString registre, CString datatype)
{	
	Send ("DAT0="+registre+" "+Champ(datatype));
	Send ("D0=D0+ "+IntToStr(Size(datatype)));
}

void CMacroCompile::LectureMemoire (CString registre, CString datatype)
{	
	Send (registre+"=DAT1 "+Champ(datatype));
}

void CMacroCompile::EcritureMemoire (CString registre, CString datatype)
{
	Send ("DAT1="+registre+" "+Champ(datatype));
}

int CMacroCompile::Size (CString type)
{
	if (type=="BOOLEAN") return 1;
	if (type=="BYTE") return 2;
	if (type=="INTEGER") return 5;
	if (type=="REAL") return 16;
	return -1;
}

char CMacroCompile::Champ (CString type)
{
	if (type=="BOOLEAN") return 'P';
	if (type=="BYTE") return 'B';
	if (type=="INTEGER") return 'A';
	if (type=="REAL") return 'W';
	return -1;
}

